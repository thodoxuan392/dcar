idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES bluetooth task uart freertos button
                    )