
if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
install( FILES kfs_particle.png kfs_particle_flare.png kfs_particle_diastar.png                   kfs_kde.png kfs_tux.png                   kfs_letters1.png kfs_letters2.png kfs_letters.desc                   kfs_explode.ogg kfs_debris.ogg  DESTINATION  ${DATA_INSTALL_DIR}/kfiresaver )
endif(OPENGL_FOUND AND OPENGL_GLU_FOUND)

