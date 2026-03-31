# MegaMol
# Copyright (c) 2026, MegaMol Dev Team
# All rights reserved.
#

megamol_plugin(mmarrow
  BUILD_DEFAULT OFF
  DEPENDS_FEATURES
    # feature1
  DEPENDS_PLUGINS
    mmstd
    datatools
    # plugin1
    # plugin2
  )

if (mmarrow_PLUGIN_ENABLED)
  # Do additional plugin specific stuff, not covered by the generic plugin creation,
  # i.e. find_package() dependencies, additional sources 'target_sources(megamolplugin PRIVATE ...)', ....
  find_package(arrow CONFIG REQUIRED)
  find_package(Parquet CONFIG REQUIRED)
  target_link_libraries(mmarrow PRIVATE "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Arrow::arrow_static,Arrow::arrow_shared>"
                                        "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Parquet::parquet_static,Parquet::parquet_shared>")
endif ()
