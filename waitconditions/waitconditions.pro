TEMPLATE = app
QT = core gui
CONFIG -= moc app_bundle
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += waitconditions.cpp
CONFIG += qt warn_on create_prl link_prl console

# install
target.path = $$[QT_INSTALL_EXAMPLES]/threads/waitconditions
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS waitconditions.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/threads/waitconditions
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

simulator: warning(This example might not fully work on Simulator platform)
