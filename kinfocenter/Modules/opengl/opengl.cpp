/*
 *  opengl.cpp
 *
 *  Copyright (C) 2021 Ivailo Monev <xakepa10@gmail.com>
 *  Copyright (C) 2008 Ivo Anjo <knuckles@gmail.com>
 *  Copyright (C) 2004 Ilya Korniyko <k_ilya@ukr.net>
 *  Adapted from Brian Paul's glxinfo from Mesa demos (http://www.mesa3d.org)
 *  Copyright (C) 1999-2002 Brian Paul
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "config-infocenter.h" // HAVE_DRMISKMS
#include "opengl.h"

#include <KPluginFactory>
#include <KPluginLoader>

#include <kaboutdata.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <qplatformdefs.h>

// X11 includes
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <openglconfig.h>

#ifdef KCM_ENABLE_DRM
#include <xf86drm.h>
#include <xf86drmMode.h>
#endif

#ifdef KCM_ENABLE_OPENGL
// GLU includes
#include <GL/glu.h>
// OpenGL includes
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#endif

#ifdef KCM_ENABLE_EGL
#include <EGL/egl.h>
#endif

#include "moc_opengl.cpp"

/*
    TODO:
        - separate opengl/glx/glu info query into functions
        - Direct Rendering must be first
*/


K_PLUGIN_FACTORY(KCMOpenGLFactory,
    registerPlugin<KCMOpenGL>();
)
K_EXPORT_PLUGIN(KCMOpenGLFactory("kcmopengl"))

// FIXME: Temporary!
bool GetInfo_OpenGL(QTreeWidget *treeWidget);

KCMOpenGL::KCMOpenGL(QWidget *parent, const QVariantList &)
    : KCModule(KCMOpenGLFactory::componentData(), parent)
{
    setupUi(this);
    layout()->setMargin(0);
    
    GetInfo_OpenGL(glinfoTreeWidget);

    // Watch for expanded and collapsed events, to resize columns
    connect(glinfoTreeWidget, SIGNAL(expanded(QModelIndex)), this, SLOT(treeWidgetChanged()));
    connect(glinfoTreeWidget, SIGNAL(collapsed(QModelIndex)), this, SLOT(treeWidgetChanged()));
    
    KAboutData *about =
    new KAboutData(I18N_NOOP("kcmopengl"), 0,
        ki18n("KCM OpenGL Information"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("(c) 2021 Ivailo Monev\n(c) 2008 Ivo Anjo\n(c) 2004 Ilya Korniyko\n(c) 1999-2002 Brian Paul"));

    about->addAuthor(ki18n("Ivailo Monev"), ki18n("Current Maintainer"), "xakepa10@gmail.com");
    about->addAuthor(ki18n("Ivo Anjo"), KLocalizedString(), "knuckles@gmail.com");
    about->addAuthor(ki18n("Ilya Korniyko"), KLocalizedString(), "k_ilya@ukr.net");
    about->addCredit(ki18n("Helge Deller"), ki18n("Original Maintainer"), "deller@gmx.de");
    about->addCredit(ki18n("Brian Paul"), ki18n("Author of glxinfo Mesa demos (http://www.mesa3d.org)"));
    setAboutData(about);
}

void KCMOpenGL::treeWidgetChanged() {
    glinfoTreeWidget->resizeColumnToContents(0);
    glinfoTreeWidget->resizeColumnToContents(1);
}

QTreeWidgetItem *newItem(QTreeWidgetItem *parent, QTreeWidgetItem *preceding, QString textCol1, QString textCol2 = QString()) {
    QTreeWidgetItem *newItem;
    if ((parent == NULL) && (preceding == NULL)) {
        newItem = new QTreeWidgetItem();
    } else if (preceding == NULL) {
        newItem = new QTreeWidgetItem(parent);
    } else {
        newItem = new QTreeWidgetItem(parent, preceding);
    }
    newItem->setText(0, textCol1);
    if (!textCol2.isNull()) {
        newItem->setText(1, textCol2);
    }
    
    newItem->setFlags(Qt::ItemIsEnabled);
    return newItem;
}

QTreeWidgetItem *newItem(QTreeWidgetItem *parent, QString textCol1, QString textCol2 = QString()) {
    return newItem(parent, NULL, textCol1, textCol2);
}

static const int scrnum = 0;

static bool IsDirect = false;

static struct {
#ifdef KCM_ENABLE_OPENGL
    const char *serverVendor;
    const char *serverVersion;
    const char *serverExtensions;
    const char *clientVendor;
    const char *clientVersion;
    const char *clientExtensions;
    const char *glxExtensions;
    const char *gluVersion;
    const char *gluExtensions;
    const char *glVendor;
    const char *glRenderer;
    const char *glVersion;
    const char *glExtensions;
#endif
#ifdef KCM_ENABLE_EGL
    const char *eglVendor;
    const char *eglVersion;
    const char *eglExtensions;
#endif
} gl_info;

static void print_extension_list(const char *ext, QTreeWidgetItem *l1)
{
    if (!ext || !ext[0])
        return;

    const QString qext = QString::fromLatin1(ext);
    QTreeWidgetItem *l2 = NULL;

    QStringList qexts = qext.split(QLatin1String(" "), QString::SkipEmptyParts);
    qSort(qexts);
    foreach(const QString &it, qexts) {
        if (!l2) {
            l2 = newItem(l1, it);
        } else {
            l2 = newItem(l1, it);
        }
    }
}

static QTreeWidgetItem *print_drm_info(QTreeWidgetItem *l1, QTreeWidgetItem *after, const QString &title)
{
#ifdef KCM_ENABLE_DRM
    QTreeWidgetItem *l2 = NULL, *l3 = NULL;

    if (after) {
        l1 = newItem(l1, after, title);
    } else {
        l1 = newItem(l1, title);
    }

    l1->setExpanded(true);

#ifndef MAX3
// libdrm DRM_NODE_NAME_MAX() macro references MAX3() macro that is not defined anywhere
#  define MAX3(A, B, C) A + B + C
#  define UNDEFINE_MAX3
#endif
    for (int i = 0; i < DRM_NODE_MAX; i++) {
        char driDevBuff[DRM_NODE_NAME_MAX];
        ::snprintf(driDevBuff, sizeof(driDevBuff), DRM_DEV_NAME, DRM_DIR_NAME, i);
#ifdef O_CLOEXEC
        int driFd = QT_OPEN(driDevBuff, O_RDWR | O_CLOEXEC, 0);
#else
        int driFd = QT_OPEN(driDevBuff, O_RDWR, 0);
#endif
        if (driFd < 0) {
            // depends on how many devices are available
            kDebug() << "QT_OPEN() failed for" << i;
            continue;
        }

        drmVersionPtr driVer = drmGetVersion(driFd);
        if (!driVer) {
            kWarning() << "drmGetVersion() failed for" << i;
            drmClose(driFd);
            continue;
        }

        const char* driBus = drmGetBusid(driFd);

        QString dri_name = QString::fromLatin1(driVer->name, driVer->name_len);
        QString dri_description = QString::fromLatin1(driVer->desc, driVer->desc_len);
        QString dri_version = QString::fromLatin1("%1.%2.%3").arg(driVer->version_major).arg(driVer->version_minor).arg(driVer->version_patchlevel);
        QString dri_bus = QString::fromLatin1(driBus);
#ifdef HAVE_DRMISKMS
        bool dri_kms = (drmIsKMS(driFd) == 1);
#endif

        drmFreeBusid(driBus);
        drmFreeVersion(driVer);
        drmClose(driFd);

        l2 = newItem(l1, i18n("Device %1", i));
        l2->setExpanded(true);
        l3 = newItem(l2, l3, i18n("Name"), dri_name);
        l3 = newItem(l2, l3, i18n("Description"), dri_description);
        l3 = newItem(l2, l3, i18n("Version"), dri_version);
        l3 = newItem(l2, l3, i18n("Bus"), dri_bus);
#ifdef HAVE_DRMISKMS
        l3 = newItem(l2, l3, i18n("Kernel mode-setting"), dri_kms ? i18n("Yes") : i18n("No"));
#endif
    }

#ifdef UNDEFINE_MAX3
#  undef MAX3
#endif
#endif // KCM_ENABLE_DRM

    return l1;
}
#ifdef KCM_ENABLE_OPENGL
#if defined(GLX_ARB_get_proc_address) && defined(__GLXextFuncPtr)
extern "C" {
    extern __GLXextFuncPtr glXGetProcAddressARB (const GLubyte *);
}
#endif

static void
mesa_hack(Display *dpy)
{
    int attribs[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DEPTH_SIZE, 1,
        GLX_STENCIL_SIZE, 1,
        GLX_ACCUM_RED_SIZE, 1,
        GLX_ACCUM_GREEN_SIZE, 1,
        GLX_ACCUM_BLUE_SIZE, 1,
        GLX_ACCUM_ALPHA_SIZE, 1,
        GLX_DOUBLEBUFFER,
        None
    };

    XVisualInfo *visinfo;

    visinfo = glXChooseVisual(dpy, scrnum, attribs);
    if (visinfo)
        XFree(visinfo);
}

static void
print_limits(QTreeWidgetItem *l1, const char * glExtensions, bool getProcAddress)
{
 /*  TODO
      GL_SAMPLE_BUFFERS
      GL_SAMPLES
      GL_COMPRESSED_TEXTURE_FORMATS
*/

    if (!glExtensions)
        return;

    struct token_name {
        GLuint type;  // count and flags, !!! count must be <=2 for now
        GLenum token;
        const QString name;
    };

    struct token_group {
        int count;
        int type;
        const token_name *group;
        const QString descr;
        const char *ext;
    };

    QTreeWidgetItem *l2 = NULL, *l3 = NULL;
    PFNGLGETPROGRAMIVARBPROC kcm_glGetProgramivARB = NULL;

    #define KCMGL_FLOAT 128
    #define KCMGL_PROG 256
    #define KCMGL_COUNT_MASK(x) (x & 127)
    #define KCMGL_SIZE(x) (sizeof(x)/sizeof(x[0]))

    const struct token_name various_limits[] = {
        { 1, GL_MAX_LIGHTS,               i18n("Max. number of light sources") },
        { 1, GL_MAX_CLIP_PLANES,          i18n("Max. number of clipping planes") },
        { 1, GL_MAX_PIXEL_MAP_TABLE,      i18n("Max. pixel map table size") },
        { 1, GL_MAX_LIST_NESTING,         i18n("Max. display list nesting level") },
        { 1, GL_MAX_EVAL_ORDER,           i18n("Max. evaluator order") },
        { 1, GL_MAX_ELEMENTS_VERTICES,    i18n("Max. recommended vertex count") },
        { 1, GL_MAX_ELEMENTS_INDICES,     i18n("Max. recommended index count") },
#ifdef GL_QUERY_COUNTER_BITS
        { 1, GL_QUERY_COUNTER_BITS,       i18n("Occlusion query counter bits")},
#endif
#ifdef GL_MAX_VERTEX_UNITS_ARB
        { 1, GL_MAX_VERTEX_UNITS_ARB,     i18n("Max. vertex blend matrices") },
#endif
#ifdef GL_MAX_PALETTE_MATRICES_ARB
        { 1, GL_MAX_PALETTE_MATRICES_ARB, i18n("Max. vertex blend matrix palette size") },
#endif
        { 0, 0, QString() }
    };

   const struct token_name texture_limits[] = {
        { 1, GL_MAX_TEXTURE_SIZE,                       i18n("Max. texture size") },
        { 1, GL_MAX_TEXTURE_UNITS_ARB,                  i18n("No. of texture units") },
        { 1, GL_MAX_3D_TEXTURE_SIZE,                    i18n("Max. 3D texture size") },
        { 1, GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB,          i18n("Max. cube map texture size") },
#ifdef GL_MAX_RECTANGLE_TEXTURE_SIZE_NV
        { 1, GL_MAX_RECTANGLE_TEXTURE_SIZE_NV,          i18n("Max. rectangular texture size") },
#endif
        { 1 | KCMGL_FLOAT, GL_MAX_TEXTURE_LOD_BIAS_EXT, i18n("Max. texture LOD bias") },
        { 1, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,         i18n("Max. anisotropy filtering level") },
        { 1, GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB,     i18n("No. of compressed texture formats") },
        { 0, 0, QString() }
    };

    const struct token_name float_limits[] = {
        { 2 | KCMGL_FLOAT, GL_ALIASED_POINT_SIZE_RANGE,      "ALIASED_POINT_SIZE_RANGE" },
        { 2 | KCMGL_FLOAT, GL_SMOOTH_POINT_SIZE_RANGE,       "SMOOTH_POINT_SIZE_RANGE" },
        { 1 | KCMGL_FLOAT, GL_SMOOTH_POINT_SIZE_GRANULARITY, "SMOOTH_POINT_SIZE_GRANULARITY"},
        { 2 | KCMGL_FLOAT, GL_ALIASED_LINE_WIDTH_RANGE,      "ALIASED_LINE_WIDTH_RANGE" },
        { 2 | KCMGL_FLOAT, GL_SMOOTH_LINE_WIDTH_RANGE,       "SMOOTH_LINE_WIDTH_RANGE" },
        { 1 | KCMGL_FLOAT, GL_SMOOTH_LINE_WIDTH_GRANULARITY, "SMOOTH_LINE_WIDTH_GRANULARITY"},
        { 0, 0, QString() }
    };

    const struct token_name stack_depth[] = {
        { 1, GL_MAX_MODELVIEW_STACK_DEPTH,          "MAX_MODELVIEW_STACK_DEPTH" },
        { 1, GL_MAX_PROJECTION_STACK_DEPTH,         "MAX_PROJECTION_STACK_DEPTH" },
        { 1, GL_MAX_TEXTURE_STACK_DEPTH,            "MAX_TEXTURE_STACK_DEPTH" },
        { 1, GL_MAX_NAME_STACK_DEPTH,               "MAX_NAME_STACK_DEPTH" },
        { 1, GL_MAX_ATTRIB_STACK_DEPTH,             "MAX_ATTRIB_STACK_DEPTH" },
        { 1, GL_MAX_CLIENT_ATTRIB_STACK_DEPTH,      "MAX_CLIENT_ATTRIB_STACK_DEPTH" },
        { 1, GL_MAX_COLOR_MATRIX_STACK_DEPTH,       "MAX_COLOR_MATRIX_STACK_DEPTH" },
#ifdef GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB
        { 1, GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB, "MAX_MATRIX_PALETTE_STACK_DEPTH"},
#endif
        { 0, 0, QString() }
    };

#ifdef GL_ARB_fragment_program
    const struct token_name arb_fp[] = {
        { 1, GL_MAX_TEXTURE_COORDS_ARB,                               "MAX_TEXTURE_COORDS" },
        { 1, GL_MAX_TEXTURE_IMAGE_UNITS_ARB,                          "MAX_TEXTURE_IMAGE_UNITS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,          "MAX_PROGRAM_ENV_PARAMETERS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,        "MAX_PROGRAM_LOCAL_PARAMETERS" },
        { 1, GL_MAX_PROGRAM_MATRICES_ARB,                             "MAX_PROGRAM_MATRICES" },
        { 1, GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB,                   "MAX_PROGRAM_MATRIX_STACK_DEPTH" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,            "MAX_PROGRAM_INSTRUCTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB,        "MAX_PROGRAM_ALU_INSTRUCTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB,        "MAX_PROGRAM_TEX_INSTRUCTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB,        "MAX_PROGRAM_TEX_INDIRECTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_TEMPORARIES_ARB,             "MAX_PROGRAM_TEMPORARIES" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_PARAMETERS_ARB,              "MAX_PROGRAM_PARAMETERS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ATTRIBS_ARB,                 "MAX_PROGRAM_ATTRIBS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,     "MAX_PROGRAM_NATIVE_INSTRUCTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, "MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, "MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, "MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,      "MAX_PROGRAM_NATIVE_TEMPORARIES" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,       "MAX_PROGRAM_NATIVE_PARAMETERS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,          "MAX_PROGRAM_NATIVE_ATTRIBS" },
        { 0, 0, QString() }
    };
#endif

#ifdef GL_ARB_vertex_program
    const struct token_name arb_vp[] = {
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,           "MAX_PROGRAM_ENV_PARAMETERS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,         "MAX_PROGRAM_LOCAL_PARAMETERS" },
        { 1, GL_MAX_VERTEX_ATTRIBS_ARB,                                "MAX_VERTEX_ATTRIBS" },
        { 1, GL_MAX_PROGRAM_MATRICES_ARB,                              "MAX_PROGRAM_MATRICES" },
        { 1, GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB,                    "MAX_PROGRAM_MATRIX_STACK_DEPTH" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,             "MAX_PROGRAM_INSTRUCTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_TEMPORARIES_ARB,              "MAX_PROGRAM_TEMPORARIES" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_PARAMETERS_ARB,               "MAX_PROGRAM_PARAMETERS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ATTRIBS_ARB,                  "MAX_PROGRAM_ATTRIBS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB,        "MAX_PROGRAM_ADDRESS_REGISTERS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,      "MAX_PROGRAM_NATIVE_INSTRUCTIONS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,       "MAX_PROGRAM_NATIVE_TEMPORARIES" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,        "MAX_PROGRAM_NATIVE_PARAMETERS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,           "MAX_PROGRAM_NATIVE_ATTRIBS" },
        { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, "MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS" },
        { 0, 0, QString() }
    };
#endif

#ifdef GL_ARB_vertex_shader
    const struct token_name arb_vs[] = {
        { 1, GL_MAX_VERTEX_ATTRIBS_ARB,               "MAX_VERTEX_ATTRIBS" },
        { 1, GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB,    "MAX_VERTEX_UNIFORM_COMPONENTS" },
        { 1, GL_MAX_VARYING_FLOATS_ARB,               "MAX_VARYING_FLOATS" },
        { 1, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, "MAX_COMBINED_TEXTURE_IMAGE_UNITS" },
        { 1, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB,   "MAX_VERTEX_TEXTURE_IMAGE_UNITS" },
        { 1, GL_MAX_TEXTURE_IMAGE_UNITS_ARB,          "MAX_TEXTURE_IMAGE_UNITS" },
        { 1, GL_MAX_TEXTURE_COORDS_ARB,               "MAX_TEXTURE_COORDS" },
        { 0, 0, QString() }
    };
#endif

#ifdef GL_ARB_fragment_shader
    const struct token_name arb_fs[] = {
        { 1, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB, "MAX_FRAGMENT_UNIFORM_COMPONENTS" },
        { 1, GL_MAX_TEXTURE_IMAGE_UNITS_ARB,         "MAX_TEXTURE_IMAGE_UNITS" },
        { 1, GL_MAX_TEXTURE_COORDS_ARB,              "MAX_TEXTURE_COORDS" },
        { 0, 0, QString() }
    };
#endif

    const struct token_name frame_buffer_props[] = {
        { 2, GL_MAX_VIEWPORT_DIMS, i18n("Max. viewport dimensions") },
        { 1, GL_SUBPIXEL_BITS,     i18n("Subpixel bits") },
        { 1, GL_AUX_BUFFERS,       i18n("Aux. buffers") },
        { 0, 0, QString() }
    };

    const struct token_group groups[] = {
        { KCMGL_SIZE(frame_buffer_props), 0,                       frame_buffer_props, i18n("Frame buffer properties"), NULL },
        { KCMGL_SIZE(various_limits),     0,                       texture_limits,     i18n("Texturing"),               NULL },
        { KCMGL_SIZE(various_limits),     0,                       various_limits,     i18n("Various limits"),          NULL },
        { KCMGL_SIZE(float_limits),       0,                       float_limits,       i18n("Points and lines"),        NULL },
        { KCMGL_SIZE(stack_depth),        0,                       stack_depth,        i18n("Stack depth limits"),      NULL },
#ifdef GL_ARB_vertex_program
        { KCMGL_SIZE(arb_vp),             GL_VERTEX_PROGRAM_ARB,   arb_vp,             "ARB_vertex_program",            "GL_ARB_vertex_program" },
#endif
#ifdef GL_ARB_fragment_program
        { KCMGL_SIZE(arb_fp),             GL_FRAGMENT_PROGRAM_ARB, arb_fp,             "ARB_fragment_program",          "GL_ARB_fragment_program" },
#endif
#ifdef GL_ARB_vertex_shader
        { KCMGL_SIZE(arb_vs),             0,                       arb_vs,             "ARB_vertex_shader",             "GL_ARB_vertex_shader" },
#endif
#ifdef GL_ARB_fragment_shader
        { KCMGL_SIZE(arb_fs),             0,                       arb_fs,             "ARB_fragment_shader",           "GL_ARB_fragment_shader" },
#endif
    };

#if defined(GLX_ARB_get_proc_address)
    if (getProcAddress && strstr(glExtensions, "GL_ARB_vertex_program"))
        kcm_glGetProgramivARB = (PFNGLGETPROGRAMIVARBPROC) glXGetProcAddressARB((const GLubyte *)"glGetProgramivARB");
#else
    Q_UNUSED(getProcAddress);
#endif

    for (uint i = 0; i < KCMGL_SIZE(groups); i++) {
        if (groups[i].ext && !strstr(glExtensions, groups[i].ext)) continue;

        if (l2) {
            l2 = newItem(l1, l2, groups[i].descr);
        } else {
            l2 = newItem(l1, groups[i].descr);
        }
        l3 = NULL;
        const struct token_name *cur_token;
        for (cur_token = groups[i].group; cur_token->type; cur_token++) {
            bool tfloat = cur_token->type & KCMGL_FLOAT;
            int count = KCMGL_COUNT_MASK(cur_token->type);
            GLint max[2] = { 0, 0 };
            GLfloat fmax[2] = { 0.0,0.0 };

#if defined(GL_ARB_vertex_program)
            bool tprog = cur_token->type & KCMGL_PROG;
            if (tprog && kcm_glGetProgramivARB) {
                kcm_glGetProgramivARB(groups[i].type, cur_token->token, max);
            } else
#endif
            if (tfloat) {
                glGetFloatv(cur_token->token, fmax);
            } else {
                glGetIntegerv(cur_token->token, max);
            }

            if (glGetError() == GL_NONE) {
                QString s;
                if (!tfloat && count == 1) {
                    s = QString::number(max[0]);
                } else if (!tfloat && count == 2) {
                    s = QString("%1, %2").arg(max[0]).arg(max[1]);
                } else if (tfloat && count == 2) {
                    s = QString("%1 - %2").arg(fmax[0],0,'f',6).arg(fmax[1],0,'f',6);
                } else if (tfloat && count == 1) {
                    s = QString::number(fmax[0],'f',6);
                }
                if (l3) {
                    l3 = newItem(l2, l3, cur_token->name, s);
                } else {
                    l3 = newItem(l2, cur_token->name, s);
                }
            }
        }

    }
}

QTreeWidgetItem *print_glx_glu(QTreeWidgetItem *l1, QTreeWidgetItem *l2)
{
    QTreeWidgetItem *l3;

    l2 = newItem(l1, l2, i18n("GLX"));
    l3 = newItem(l2, i18n("server GLX vendor"), gl_info.serverVendor);
    l3 = newItem(l2, l3, i18n("server GLX version"), gl_info.serverVersion);
    l3 = newItem(l2, l3, i18n("server GLX extensions"));
    print_extension_list(gl_info.serverExtensions, l3);

    l3 = newItem(l2, l3, i18n("client GLX vendor") ,gl_info.clientVendor);
    l3 = newItem(l2, l3, i18n("client GLX version"), gl_info.clientVersion);
    l3 = newItem(l2, l3, i18n("client GLX extensions"));
    print_extension_list(gl_info.clientExtensions, l3);
    l3 = newItem(l2, l3, i18n("GLX extensions"));
    print_extension_list(gl_info.glxExtensions, l3);

    l2 = newItem(l1, l2, i18n("GLU"));
    l3 = newItem(l2, i18n("GLU version"), gl_info.gluVersion);
    l3 = newItem(l2, l3, i18n("GLU extensions"));
    print_extension_list(gl_info.gluExtensions, l3);

    l2 = newItem(l1, l2, i18n("OpenGL"));
    l3 = newItem(l2, i18n("Vendor"), gl_info.glVendor);
    l3 = newItem(l2, l3, i18n("Renderer"), gl_info.glRenderer);
    l3 = newItem(l2, l3, i18n("OpenGL version"), gl_info.glVersion);
    l3 = newItem(l2, l3, i18n("OpenGL extensions"));
    print_extension_list(gl_info.glExtensions, l3);

    l3 = newItem(l2, l3, i18n("Implementation specific"));
    print_limits(l3, gl_info.glExtensions, strstr(gl_info.clientExtensions, "GLX_ARB_get_proc_address") != NULL);

    return l1;
}

static QTreeWidgetItem *get_gl_info(Display *dpy, QTreeWidgetItem *l1, QTreeWidgetItem *after)
{
    Window win;
    XSetWindowAttributes attr;
    unsigned long mask;
    Window root;
    XVisualInfo *visinfo;
    int width = 100, height = 100;
    QTreeWidgetItem *result = after;

    root = RootWindow(dpy, scrnum);

    int attribSingle[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        None
    };
    int attribDouble[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER,
        None
    };
    GLXContext ctx;

    visinfo = glXChooseVisual(dpy, scrnum, attribSingle);
    if (!visinfo) {
        visinfo = glXChooseVisual(dpy, scrnum, attribDouble);
        if (!visinfo) {
            kWarning() << "couldn't find RGB GLX visual";
            return result;
        }
    }

    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask;
    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
    win = XCreateWindow(dpy, root, 0, 0, width, height,
                        0, visinfo->depth, InputOutput,
                        visinfo->visual, mask, &attr);

    ctx = glXCreateContext( dpy, visinfo, NULL, true );
    if (!ctx) {
        kWarning() << "glXCreateContext failed";
        XDestroyWindow(dpy, win);
        XFree(visinfo);
        return result;
    }

    if (glXMakeCurrent(dpy, win, ctx)) {
        gl_info.serverVendor = glXQueryServerString(dpy, scrnum, GLX_VENDOR);
        gl_info.serverVersion = glXQueryServerString(dpy, scrnum, GLX_VERSION);
        gl_info.serverExtensions = glXQueryServerString(dpy, scrnum, GLX_EXTENSIONS);
        gl_info.clientVendor = glXGetClientString(dpy, GLX_VENDOR);
        gl_info.clientVersion = glXGetClientString(dpy, GLX_VERSION);
        gl_info.clientExtensions = glXGetClientString(dpy, GLX_EXTENSIONS);
        gl_info.glxExtensions = glXQueryExtensionsString(dpy, scrnum);
        gl_info.glVendor = (const char *) glGetString(GL_VENDOR);
        gl_info.glRenderer = (const char *) glGetString(GL_RENDERER);
        gl_info.glVersion = (const char *) glGetString(GL_VERSION);
        gl_info.glExtensions = (const char *) glGetString(GL_EXTENSIONS);
        gl_info.gluVersion = (const char *) gluGetString(GLU_VERSION);
        gl_info.gluExtensions = (const char *) gluGetString(GLU_EXTENSIONS);

        IsDirect = glXIsDirect(dpy, ctx);

        result = print_glx_glu(l1, after);
    } else {
        kWarning() << "glXMakeCurrent failed";
    }

    glXDestroyContext(dpy, ctx);

    XDestroyWindow(dpy, win);
    XFree(visinfo);
    return result;
}
#endif // KCM_ENABLE_OPENGL

#ifdef KCM_ENABLE_EGL
QTreeWidgetItem *print_egl(QTreeWidgetItem *l1, QTreeWidgetItem *l2)
{
    QTreeWidgetItem *l3;

    l2 = newItem(l1, l2, i18n("EGL"));
    l3 = newItem(l2, i18n("EGL Vendor"), gl_info.eglVendor);
    l3 = newItem(l2, l3, i18n("EGL Version"), gl_info.eglVersion);
    l3 = newItem(l2, l3, i18n("EGL Extensions"));
    print_extension_list(gl_info.eglExtensions, l3);

    return l1;
}

static QTreeWidgetItem *get_egl_info(Display *dpy, QTreeWidgetItem *l1, QTreeWidgetItem *after)
{
    QTreeWidgetItem *result = after;

    EGLDisplay egl_dpy;
    EGLint egl_major, egl_minor;

    egl_dpy = eglGetDisplay(dpy);
    if (!egl_dpy) {
        kWarning() << "eglGetDisplay() failed";
        return result;
    }

    if (!eglInitialize(egl_dpy, &egl_major, &egl_minor)) {
        kWarning() << "eglInitialize() failed";
        eglTerminate(egl_dpy);
        return result;
    }

    gl_info.eglVendor = eglQueryString(egl_dpy, EGL_VENDOR);
    gl_info.eglVersion = eglQueryString(egl_dpy, EGL_VERSION);
    gl_info.eglExtensions = eglQueryString(egl_dpy, EGL_EXTENSIONS);

    result = print_egl(l1, after);

    eglTerminate(egl_dpy);
    return result;
}
#endif // KCM_ENABLE_EGL

bool GetInfo_OpenGL(QTreeWidget *treeWidget)
{
    QTreeWidgetItem *l1, *l2 = NULL;

    Display *dpy;

    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        kWarning() << "unable to open display";
        return false;
    }

    QTreeWidgetItem *header = new QTreeWidgetItem();
    header->setText(0, i18n("Information"));
    header->setText(1, i18n("Value"));
    treeWidget->setHeaderItem(header);

    treeWidget->setRootIsDecorated(false);

    l1 = new QTreeWidgetItem(treeWidget);
    l1->setText(0, i18n("Name of the Display"));
    l1->setText(1, DisplayString(dpy));
    l1->setExpanded(true);
    l1->setFlags(Qt::ItemIsEnabled);


#if defined(KCM_ENABLE_DRM)
    const int driAvail = drmAvailable();
    // qDebug() << "driAvail" << driAvail;
    if (driAvail) {
        l2 = print_drm_info(l1, l2, i18n("Direct Rendering"));
    }
#endif

    // TODO: print_visual_info(dpy, mode);

#ifdef KCM_ENABLE_OPENGL
    mesa_hack(dpy);

    l2 = get_gl_info(dpy, l1, l2);
    if (!l2)
        KMessageBox::error(0, i18n("Could not initialize OpenGL"));
#endif

    if (l2)
        l2->setExpanded(true);

#ifdef KCM_ENABLE_EGL
    l2 = get_egl_info(dpy, l1, l2);
    if (!l2)
        KMessageBox::error(0, i18n("Could not initialize OpenGL ES2.0"));
#endif

    treeWidget->resizeColumnToContents(0);
    treeWidget->resizeColumnToContents(1);

    XCloseDisplay(dpy);
    return true;
}
