import qbs 1.0

QtcPlugin {
    name: "AutotoolsProjectManager"

    Depends { name: "Qt.widgets" }
    Depends { name: "Utils" }

    Depends { name: "Core" }
    Depends { name: "ProjectExplorer" }
    Depends { name: "CppTools" }

    files: [
        "autogenstep.cpp",
        "autogenstep.h",
        "autoreconfstep.cpp",
        "autoreconfstep.h",
        "autotoolsbuildconfiguration.cpp",
        "autotoolsbuildconfiguration.h",
        "autotoolsbuildsettingswidget.cpp",
        "autotoolsbuildsettingswidget.h",
        "autotoolsmanager.cpp",
        "autotoolsmanager.h",
        "autotoolsopenprojectwizard.cpp",
        "autotoolsopenprojectwizard.h",
        "autotoolsproject.cpp",
        "autotoolsproject.h",
        "autotoolsprojectconstants.h",
        "autotoolsprojectfile.cpp",
        "autotoolsprojectfile.h",
        "autotoolsprojectnode.cpp",
        "autotoolsprojectnode.h",
        "autotoolsprojectplugin.cpp",
        "autotoolsprojectplugin.h",
        "configurestep.cpp",
        "configurestep.h",
        "makefileparser.cpp",
        "makefileparser.h",
        "makefileparserthread.cpp",
        "makefileparserthread.h",
        "makestep.cpp",
        "makestep.h",
    ]
}
