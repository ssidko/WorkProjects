/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QCameraViewfinder"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *CreateReportAction;
    QAction *OpenAction;
    QAction *SaveAction;
    QAction *PropertiesAction;
    QAction *CreateAction;
    QAction *AboutAction;
    QAction *HelpAction;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *TaskTreeWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *ScreenGroupBox;
    QGridLayout *gridLayout;
    QCameraViewfinder *ScreenWidget;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *ButtonsGroupBox;
    QHBoxLayout *ButtonsHLayout;
    QPushButton *ScreenshotButton;
    QMenuBar *menubar;
    QMenu *TaskMenu;
    QMenu *ReportMenu;
    QMenu *HelpMenu;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(942, 635);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        MainWindow->setMaximumSize(QSize(16777215, 635));
        MainWindow->setLocale(QLocale(QLocale::Russian, QLocale::Russia));
        CreateReportAction = new QAction(MainWindow);
        CreateReportAction->setObjectName(QStringLiteral("CreateReportAction"));
        OpenAction = new QAction(MainWindow);
        OpenAction->setObjectName(QStringLiteral("OpenAction"));
        SaveAction = new QAction(MainWindow);
        SaveAction->setObjectName(QStringLiteral("SaveAction"));
        PropertiesAction = new QAction(MainWindow);
        PropertiesAction->setObjectName(QStringLiteral("PropertiesAction"));
        CreateAction = new QAction(MainWindow);
        CreateAction->setObjectName(QStringLiteral("CreateAction"));
        AboutAction = new QAction(MainWindow);
        AboutAction->setObjectName(QStringLiteral("AboutAction"));
        HelpAction = new QAction(MainWindow);
        HelpAction->setObjectName(QStringLiteral("HelpAction"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        TaskTreeWidget = new QTreeWidget(centralwidget);
        TaskTreeWidget->setObjectName(QStringLiteral("TaskTreeWidget"));
        TaskTreeWidget->setColumnCount(0);
        TaskTreeWidget->header()->setVisible(false);

        horizontalLayout->addWidget(TaskTreeWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        ScreenGroupBox = new QGroupBox(centralwidget);
        ScreenGroupBox->setObjectName(QStringLiteral("ScreenGroupBox"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        ScreenGroupBox->setFont(font);
        ScreenGroupBox->setAlignment(Qt::AlignCenter);
        gridLayout = new QGridLayout(ScreenGroupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        ScreenWidget = new QCameraViewfinder(ScreenGroupBox);
        ScreenWidget->setObjectName(QStringLiteral("ScreenWidget"));
        ScreenWidget->setMinimumSize(QSize(640, 480));
        ScreenWidget->setMaximumSize(QSize(640, 480));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(170, 170, 127, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        ScreenWidget->setPalette(palette);
        QFont font1;
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setKerning(true);
        ScreenWidget->setFont(font1);
        ScreenWidget->setAutoFillBackground(true);
        ScreenWidget->setLocale(QLocale(QLocale::Russian, QLocale::Ukraine));

        gridLayout->addWidget(ScreenWidget, 0, 0, 1, 1);


        verticalLayout->addWidget(ScreenGroupBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        ButtonsGroupBox = new QGroupBox(centralwidget);
        ButtonsGroupBox->setObjectName(QStringLiteral("ButtonsGroupBox"));
        ButtonsHLayout = new QHBoxLayout(ButtonsGroupBox);
        ButtonsHLayout->setObjectName(QStringLiteral("ButtonsHLayout"));

        horizontalLayout_2->addWidget(ButtonsGroupBox);

        ScreenshotButton = new QPushButton(centralwidget);
        ScreenshotButton->setObjectName(QStringLiteral("ScreenshotButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(ScreenshotButton->sizePolicy().hasHeightForWidth());
        ScreenshotButton->setSizePolicy(sizePolicy1);
        ScreenshotButton->setMinimumSize(QSize(100, 50));
        ScreenshotButton->setMaximumSize(QSize(100, 50));
        QIcon icon;
        icon.addFile(QStringLiteral(":/MainWindow/images/camera.png"), QSize(), QIcon::Normal, QIcon::Off);
        ScreenshotButton->setIcon(icon);
        ScreenshotButton->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(ScreenshotButton);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 942, 21));
        TaskMenu = new QMenu(menubar);
        TaskMenu->setObjectName(QStringLiteral("TaskMenu"));
        ReportMenu = new QMenu(menubar);
        ReportMenu->setObjectName(QStringLiteral("ReportMenu"));
        HelpMenu = new QMenu(menubar);
        HelpMenu->setObjectName(QStringLiteral("HelpMenu"));
        MainWindow->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        statusBar->setSizeGripEnabled(false);
        MainWindow->setStatusBar(statusBar);

        menubar->addAction(TaskMenu->menuAction());
        menubar->addAction(ReportMenu->menuAction());
        menubar->addAction(HelpMenu->menuAction());
        TaskMenu->addAction(CreateAction);
        TaskMenu->addAction(OpenAction);
        TaskMenu->addAction(SaveAction);
        TaskMenu->addSeparator();
        TaskMenu->addAction(PropertiesAction);
        ReportMenu->addAction(CreateReportAction);
        HelpMenu->addAction(HelpAction);
        HelpMenu->addSeparator();
        HelpMenu->addAction(AboutAction);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        CreateReportAction->setText(QApplication::translate("MainWindow", "\320\241\321\204\320\276\321\200\320\274\320\270\321\200\320\276\320\262\320\260\321\202\321\214 \320\262 \321\204\320\276\321\200\320\274\320\260\321\202\320\265 html", 0));
        OpenAction->setText(QApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", 0));
        SaveAction->setText(QApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", 0));
        PropertiesAction->setText(QApplication::translate("MainWindow", "\320\241\320\262\320\276\320\271\321\201\321\202\320\262\320\260", 0));
        CreateAction->setText(QApplication::translate("MainWindow", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214", 0));
        AboutAction->setText(QApplication::translate("MainWindow", "\320\236 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\265", 0));
        HelpAction->setText(QApplication::translate("MainWindow", "\320\237\320\276\320\274\320\276\321\211\321\214", 0));
        ScreenGroupBox->setTitle(QApplication::translate("MainWindow", "\320\255\320\272\321\200\320\260\320\275", 0));
        ButtonsGroupBox->setTitle(QApplication::translate("MainWindow", "\320\243\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265", 0));
        ScreenshotButton->setText(QApplication::translate("MainWindow", "\320\241\320\272\321\200\320\270\320\275\321\210\320\276\321\202", 0));
        TaskMenu->setTitle(QApplication::translate("MainWindow", "\320\227\320\260\320\264\320\260\321\207\320\260", 0));
        ReportMenu->setTitle(QApplication::translate("MainWindow", "\320\236\321\202\321\207\321\221\321\202", 0));
        HelpMenu->setTitle(QApplication::translate("MainWindow", "\320\237\320\276\320\274\320\276\321\211\321\214", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
