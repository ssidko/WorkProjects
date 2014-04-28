/********************************************************************************
** Form generated from reading UI file 'NewTaskDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWTASKDIALOG_H
#define UI_NEWTASKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewTaskDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *CaseGroupBox;
    QGridLayout *gridLayout;
    QLabel *TaskNameLabel;
    QLabel *TaskDescriptionLabel;
    QLineEdit *TaskDescriptionEdit;
    QLineEdit *TaskNameEdit;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QComboBox *TemplateComboBox;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *DirectoryLineEdit;
    QPushButton *DirectoryPushButton;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *CreateTaskPushButton;
    QPushButton *CancelPushButton;

    void setupUi(QDialog *NewTaskDialog)
    {
        if (NewTaskDialog->objectName().isEmpty())
            NewTaskDialog->setObjectName(QStringLiteral("NewTaskDialog"));
        NewTaskDialog->resize(439, 249);
        verticalLayout = new QVBoxLayout(NewTaskDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        CaseGroupBox = new QGroupBox(NewTaskDialog);
        CaseGroupBox->setObjectName(QStringLiteral("CaseGroupBox"));
        gridLayout = new QGridLayout(CaseGroupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        TaskNameLabel = new QLabel(CaseGroupBox);
        TaskNameLabel->setObjectName(QStringLiteral("TaskNameLabel"));

        gridLayout->addWidget(TaskNameLabel, 0, 0, 1, 1);

        TaskDescriptionLabel = new QLabel(CaseGroupBox);
        TaskDescriptionLabel->setObjectName(QStringLiteral("TaskDescriptionLabel"));

        gridLayout->addWidget(TaskDescriptionLabel, 1, 0, 1, 2);

        TaskDescriptionEdit = new QLineEdit(CaseGroupBox);
        TaskDescriptionEdit->setObjectName(QStringLiteral("TaskDescriptionEdit"));

        gridLayout->addWidget(TaskDescriptionEdit, 1, 2, 1, 1);

        TaskNameEdit = new QLineEdit(CaseGroupBox);
        TaskNameEdit->setObjectName(QStringLiteral("TaskNameEdit"));

        gridLayout->addWidget(TaskNameEdit, 0, 2, 1, 1);


        verticalLayout->addWidget(CaseGroupBox);

        groupBox = new QGroupBox(NewTaskDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        TemplateComboBox = new QComboBox(groupBox);
        TemplateComboBox->setObjectName(QStringLiteral("TemplateComboBox"));
        TemplateComboBox->setMinimumSize(QSize(150, 0));

        horizontalLayout->addWidget(TemplateComboBox);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(NewTaskDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        DirectoryLineEdit = new QLineEdit(groupBox_2);
        DirectoryLineEdit->setObjectName(QStringLiteral("DirectoryLineEdit"));
        DirectoryLineEdit->setReadOnly(true);

        horizontalLayout_2->addWidget(DirectoryLineEdit);

        DirectoryPushButton = new QPushButton(groupBox_2);
        DirectoryPushButton->setObjectName(QStringLiteral("DirectoryPushButton"));
        DirectoryPushButton->setMaximumSize(QSize(30, 16777215));
        DirectoryPushButton->setAutoDefault(false);

        horizontalLayout_2->addWidget(DirectoryPushButton);


        verticalLayout->addWidget(groupBox_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        CreateTaskPushButton = new QPushButton(NewTaskDialog);
        CreateTaskPushButton->setObjectName(QStringLiteral("CreateTaskPushButton"));

        horizontalLayout_3->addWidget(CreateTaskPushButton);

        CancelPushButton = new QPushButton(NewTaskDialog);
        CancelPushButton->setObjectName(QStringLiteral("CancelPushButton"));
        CancelPushButton->setAutoDefault(false);

        horizontalLayout_3->addWidget(CancelPushButton);


        verticalLayout->addLayout(horizontalLayout_3);

        QWidget::setTabOrder(TaskNameEdit, TaskDescriptionEdit);
        QWidget::setTabOrder(TaskDescriptionEdit, TemplateComboBox);
        QWidget::setTabOrder(TemplateComboBox, DirectoryLineEdit);
        QWidget::setTabOrder(DirectoryLineEdit, DirectoryPushButton);
        QWidget::setTabOrder(DirectoryPushButton, CreateTaskPushButton);
        QWidget::setTabOrder(CreateTaskPushButton, CancelPushButton);

        retranslateUi(NewTaskDialog);
        QObject::connect(CancelPushButton, SIGNAL(clicked()), NewTaskDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewTaskDialog);
    } // setupUi

    void retranslateUi(QDialog *NewTaskDialog)
    {
        NewTaskDialog->setWindowTitle(QApplication::translate("NewTaskDialog", "\320\235\320\276\320\262\320\260\321\217 \320\267\320\260\320\264\320\260\321\207\320\260", 0));
        CaseGroupBox->setTitle(QApplication::translate("NewTaskDialog", "\320\227\320\260\320\264\320\260\321\207\320\260", 0));
        TaskNameLabel->setText(QApplication::translate("NewTaskDialog", "\320\230\320\274\321\217:", 0));
        TaskDescriptionLabel->setText(QApplication::translate("NewTaskDialog", "\320\236\320\277\320\270\321\201\320\260\320\275\320\270\320\265:", 0));
        TaskNameEdit->setText(QApplication::translate("NewTaskDialog", "\320\235\320\276\320\262\320\260\321\217 \320\267\320\260\320\264\320\260\321\207\320\260", 0));
        groupBox->setTitle(QApplication::translate("NewTaskDialog", "\320\242\320\270\320\277 \320\270\320\263\321\200\320\276\320\262\320\276\320\263\320\276 \320\260\320\262\321\202\320\276\320\274\320\260\321\202\320\260", 0));
        groupBox_2->setTitle(QApplication::translate("NewTaskDialog", "\320\232\320\260\321\202\320\260\320\273\320\276\320\263 \320\276\321\202\321\207\321\221\321\202\320\260", 0));
        DirectoryLineEdit->setText(QString());
        DirectoryPushButton->setText(QApplication::translate("NewTaskDialog", "...", 0));
        CreateTaskPushButton->setText(QApplication::translate("NewTaskDialog", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214", 0));
        CancelPushButton->setText(QApplication::translate("NewTaskDialog", "\320\236\321\202\320\274\320\265\320\275\320\260", 0));
    } // retranslateUi

};

namespace Ui {
    class NewTaskDialog: public Ui_NewTaskDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWTASKDIALOG_H
