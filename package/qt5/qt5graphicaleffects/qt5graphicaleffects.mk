################################################################################
#
# qt5graphicaleffects
#
################################################################################

QT5GRAPHICALEFFECTS_VERSION = $(QT5_VERSION)
QT5GRAPHICALEFFECTS_SITE = $(QT5_SITE)
QT5GRAPHICALEFFECTS_SOURCE = qtgraphicaleffects-$(QT5_SOURCE_TARBALL_PREFIX)-$(QT5GRAPHICALEFFECTS_VERSION).tar.xz
QT5GRAPHICALEFFECTS_DEPENDENCIES = qt5base qt5declarative
QT5GRAPHICALEFFECTS_INSTALL_STAGING = YES

QT5GRAPHICALEFFECTS_LICENSE = GPL-2.0+ or LGPL-3.0, GPL-3.0 with exception(tools), GFDL-1.3 (docs)
QT5GRAPHICALEFFECTS_LICENSE_FILES = LICENSE.GPL2 LICENSE.GPL3 LICENSE.GPL3-EXCEPT LICENSE.LGPL3 LICENSE.FDL

define QT5GRAPHICALEFFECTS_CONFIGURE_CMDS
	(cd $(@D); $(TARGET_MAKE_ENV) $(HOST_DIR)/bin/qmake)
endef

define QT5GRAPHICALEFFECTS_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef

define QT5GRAPHICALEFFECTS_INSTALL_STAGING_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install
endef

define QT5GRAPHICALEFFECTS_INSTALL_TARGET_CMDS
	cp -dpfr $(STAGING_DIR)/usr/qml/QtGraphicalEffects $(TARGET_DIR)/usr/qml
endef

$(eval $(generic-package))
