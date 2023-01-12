set(PY_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/pyapmidg)

install(FILES pyapmidg.py DESTINATION ${PY_INSTALL_PATH})
install(FILES keypress.py DESTINATION ${PY_INSTALL_PATH})
install(FILES clr_misc.py DESTINATION ${PY_INSTALL_PATH})
install(FILES clr_nodeinfo.py DESTINATION ${PY_INSTALL_PATH})
install(FILES clr_rapl.py DESTINATION ${PY_INSTALL_PATH})
install(PROGRAMS apmidg_monitor_demo DESTINATION bin)
install(PROGRAMS apmidg_reset_power_limit DESTINATION bin)
