set(PY_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/pyapmidg)

install(FILES pyapmidg.py DESTINATION ${PY_INSTALL_PATH})
install(FILES keypress.py DESTINATION ${PY_INSTALL_PATH})
install(FILES clr_misc.py DESTINATION ${PY_INSTALL_PATH})
install(FILES clr_nodeinfo.py DESTINATION ${PY_INSTALL_PATH})
install(FILES clr_rapl.py DESTINATION ${PY_INSTALL_PATH})
install(PROGRAMS demo_monitor_articus DESTINATION bin)
