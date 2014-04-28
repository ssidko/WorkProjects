/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
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
    QAction *InfoAction;
    QAction *CreateAction;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_3;
    QTreeWidget *TaskTreeWidget;
    QVBoxLayout *verticalLayout;
    QCameraViewfinder *ScreenWidget;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *ButtonsGroupBox;
    QHBoxLayout *ButtonsHLayout;
    QPushButton *ScreenshotButton;
    QMenuBar *menubar;
    QMenu *TaskMenu;
    QMenu *ReportMenu;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(922, 605);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setLocale(QLocale(QLocale::Russian, QLocale::Russia));
        CreateReportAction = new QAction(MainWindow);
        CreateReportAction->setObjectName(QStringLiteral("CreateReportAction"));
        OpenAction = new QAction(MainWindow);
        OpenAction->setObjectName(QStringLiteral("OpenAction"));
        SaveAction = new QAction(MainWindow);
        SaveAction->setObjectName(QStringLiteral("SaveAction"));
        InfoAction = new QAction(MainWindow);
        InfoAction->setObjectName(QStringLiteral("InfoAction"));
        CreateAction = new QAction(MainWindow);
        CreateAction->setObjectName(QStringLiteral("CreateAction"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout_3 = new QHBoxLayout(centralwidget);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        TaskTreeWidget = new QTreeWidget(centralwidget);
        TaskTreeWidget->setObjectName(QStringLiteral("TaskTreeWidget"));
        TaskTreeWidget->setColumnCount(0);

        horizontalLayout_3->addWidget(TaskTreeWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        ScreenWidget = new QCameraViewfinder(centralwidget);
        ScreenWidget->setObjectName(QStringLiteral("ScreenWidget"));
        ScreenWidget->setMinimumSize(QSize(640, 480));
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
        ScreenWidget->setAutoFillBackground(true);
        ScreenWidget->setLocale(QLocale(QLocale::Russian, QLocale::Ukraine));

        verticalLayout->addWidget(ScreenWidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        ButtonsGroupBox = new QGroupBox(centralwidget);
        ButtonsGroupBox->setObjectName(QStringLiteral("ButtonsGroupBox"));
        ButtonsHLayout = new QHBoxLayout(ButtonsGroupBox);
        ButtonsHLayout->setObjectName(QStringLiteral("ButtonsHLayout"));

        horizontalLayout_2->addWidget(ButtonsGroupBox);

        ScreenshotButton = new QPushButton(centralwidget);
        ScreenshotButton->setObjectName(QStringLiteral("ScreenshotButton"));
        ScreenshotButton->setMinimumSize(QSize(50, 50));
        ScreenshotButton->setMaximumSize(QSize(100, 50));

        horizontalLayout_2->addWidget(ScreenshotButton);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 922, 21));
        TaskMenu = new QMenu(menubar);
        TaskMenu->setObjectName(QStringLiteral("TaskMenu"));
        ReportMenu = new QMenu(menubar);
        ReportMenu->setObjectName(QStringLiteral("ReportMenu"));
        MainWindow->setMenuBar(menubar);

        menubar->addAction(TaskMenu->menuAction());
        menubar->addAction(ReportMenu->menuAction());
        TaskMenu->addAction(CreateAction);
        TaskMenu->addAction(OpenAction);
        TaskMenu->addAction(SaveAction);
        TaskMenu->addAction(InfoAction);
        ReportMenu->addAction(CreateReportAction);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        CreateReportAction->setText(QApplication::translate("MainWindow", "\320\241\321\204\320\276\321\200\320\274\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0));
        OpenAction->setText(QApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", 0));
        SaveAction->setText(QApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", 0));
        InfoAction->setText(QApplication::translate("MainWindow", "\320\230\320\275\321\204\320\276\321\200\320\274\320\260\321\206\320\270\321\217", 0));
        CreateAction->setText(QApplication::translate("MainWindow", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214", 0));
        ButtonsGroupBox->setTitle(QApplication::translate("MainWindow", "\320\232\320\275\320\276\320\277\320\272\320\270 \321\203\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\321\217", 0));
        ScreenshotButton->setText(QApplication::translate("MainWindow", "\320\241\320\264\320\265\320\273\320\260\321\202\321\214 \321\201\320\275\320\270\320\274\320\276\320\272", 0));
        TaskMenu->setTitle(QApplication::translate("MainWindow", "\320\227\320\260\320\264\320\260\321\207\320\260", 0));
        ReportMenu->setTitle(QApplication::translate("MainWindow", "\320\236\321\202\321\207\321\221\321\202", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
