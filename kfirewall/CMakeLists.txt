project(kfirewall)

find_program(IPTABLES_EXECUTABLE iptables-restore)
add_feature_info(iptables
    IPTABLES_EXECUTABLE
    "Firewall support"
)

add_subdirectory(kcm)
add_subdirectory(kded)
