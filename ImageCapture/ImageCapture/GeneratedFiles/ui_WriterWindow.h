/********************************************************************************
** Form generated from reading UI file 'WriterWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WRITERWINDOW_H
#define UI_WRITERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WriterWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QComboBox *comboBox;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QPushButton *SelectFileButton;
    QProgressBar *progressBar;
    QPushButton *StartButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *WriterWindow)
    {
        if (WriterWindow->objectName().isEmpty())
            WriterWindow->setObjectName(QStringLiteral("WriterWindow"));
        WriterWindow->resize(275, 165);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(WriterWindow->sizePolicy().hasHeightForWidth());
        WriterWindow->setSizePolicy(sizePolicy);
        WriterWindow->setWindowTitle(QStringLiteral("I2C Writer"));
        centralwidget = new QWidget(WriterWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        comboBox = new QComboBox(centralwidget);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        verticalLayout->addWidget(comboBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout->addWidget(lineEdit);

        SelectFileButton = new QPushButton(centralwidget);
        SelectFileButton->setObjectName(QStringLiteral("SelectFileButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(10);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(SelectFileButton->sizePolicy().hasHeightForWidth());
        SelectFileButton->setSizePolicy(sizePolicy1);
        SelectFileButton->setMinimumSize(QSize(25, 20));
        SelectFileButton->setMaximumSize(QSize(25, 20));

        horizontalLayout->addWidget(SelectFileButton);


        verticalLayout->addLayout(horizontalLayout);

        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);
        progressBar->setAlignment(Qt::AlignCenter);
        progressBar->setTextVisible(true);
        progressBar->setTextDirection(QProgressBar::TopToBottom);

        verticalLayout->addWidget(progressBar);

        StartButton = new QPushButton(centralwidget);
        StartButton->setObjectName(QStringLiteral("StartButton"));

        verticalLayout->addWidget(StartButton);


        verticalLayout_2->addLayout(verticalLayout);

        WriterWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WriterWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 275, 21));
        WriterWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(WriterWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        WriterWindow->setStatusBar(statusbar);

        retranslateUi(WriterWindow);

        QMetaObject::connectSlotsByName(WriterWindow);
    } // setupUi

    void retranslateUi(QMainWindow *WriterWindow)
    {
        SelectFileButton->setText(QApplication::translate("WriterWindow", "...", 0));
        StartButton->setText(QApplication::translate("WriterWindow", "Start", 0));
        Q_UNUSED(WriterWindow);
    } // retranslateUi

};

namespace Ui {
    class WriterWindow: public Ui_WriterWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WRITERWINDOW_H
