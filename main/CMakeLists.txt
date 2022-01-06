set(COMPONENT_REQUIRES "json")

set(srcs "main.c"
	"https_request.c"
	"openweathermap.c"
    "config_helper.c"
    "sys_conf.c"
    "weather_request.c"
    "rest_server.c"
    "net.c"
    "vfs.c"
    "time_keeper.c"
    "wifi_manager.c"
    "gui.c"
    "icons/i1d.c"
    "icons/i1n.c"
    "icons/i2d.c"
    "icons/i2n.c"
    "icons/i3d.c"
    "icons/i3n.c"
    "icons/i4d.c"
    "icons/i4n.c"
    "icons/i9d.c"
    "icons/i9n.c"
    "icons/i10d.c"
    "icons/i10n.c"
    "icons/i11d.c"
    "icons/i11n.c"
    "icons/i13d.c"
    "icons/i13n.c"
    "icons/i50d.c"
    "icons/i50n.c"
    "icons/ihum.c"
    "icons/itemp.c"
    )

# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include")
if(CONFIG_OH_WEATHER_WEB_DEPLOY_SF)
    set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../front/web-demo")
    if(EXISTS ${WEB_SRC_DIR}/dist)
        spiffs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)
    else()
        message(FATAL_ERROR "${WEB_SRC_DIR}/dist doesn't exit. Please run 'npm run build' in ${WEB_SRC_DIR}")
    endif()
endif()