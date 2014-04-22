/********************************************************************************
** Form generated from reading UI file 'NewCaseDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWCASEDIALOG_H
#define UI_NEWCASEDIALOG_H

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

class Ui_NewCaseDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *CaseGroupBox;
    QGridLayout *gridLayout;
    QLabel *CaseNameLabel;
    QLabel *CaseDescriptionLabel;
    QLineEdit *CaseDescriptionLineEdit;
    QLineEdit *case_name_edit;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QComboBox *TemplateComboBox;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *DirectoryLineEdit;
    QPushButton *DirectoryPushButton;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *CreateCasePushButton;
    QPushButton *CancelPushButton;

    void setupUi(QDialog *NewCaseDialog)
    {
        if (NewCaseDialog->objectName().isEmpty())
            NewCaseDialog->setObjectName(QStringLiteral("NewCaseDialog"));
        NewCaseDialog->resize(439, 249);
        verticalLayout = new QVBoxLayout(NewCaseDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        CaseGroupBox = new QGroupBox(NewCaseDialog);
        CaseGroupBox->setObjectName(QStringLiteral("CaseGroupBox"));
        gridLayout = new QGridLayout(CaseGroupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        CaseNameLabel = new QLabel(CaseGroupBox);
        CaseNameLabel->setObjectName(QStringLiteral("CaseNameLabel"));

        gridLayout->addWidget(CaseNameLabel, 0, 0, 1, 1);

        CaseDescriptionLabel = new QLabel(CaseGroupBox);
        CaseDescriptionLabel->setObjectName(QStringLiteral("CaseDescriptionLabel"));

        gridLayout->addWidget(CaseDescriptionLabel, 1, 0, 1, 2);

        CaseDescriptionLineEdit = new QLineEdit(CaseGroupBox);
        CaseDescriptionLineEdit->setObjectName(QStringLiteral("CaseDescriptionLineEdit"));

        gridLayout->addWidget(CaseDescriptionLineEdit, 1, 2, 1, 1);

        case_name_edit = new QLineEdit(CaseGroupBox);
        case_name_edit->setObjectName(QStringLiteral("case_name_edit"));

        gridLayout->addWidget(case_name_edit, 0, 2, 1, 1);


        verticalLayout->addWidget(CaseGroupBox);

        groupBox = new QGroupBox(NewCaseDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        TemplateComboBox = new QComboBox(groupBox);
        TemplateComboBox->setObjectName(QStringLiteral("TemplateComboBox"));
        TemplateComboBox->setMinimumSize(QSize(150, 0));

        horizontalLayout->addWidget(TemplateComboBox);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(NewCaseDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        DirectoryLineEdit = new QLineEdit(groupBox_2);
        DirectoryLineEdit->setObjectName(QStringLiteral("DirectoryLineEdit"));

        horizontalLayout_2->addWidget(DirectoryLineEdit);

        DirectoryPushButton = new QPushButton(groupBox_2);
        DirectoryPushButton->setObjectName(QStringLiteral("DirectoryPushButton"));
        DirectoryPushButton->setMaximumSize(QSize(30, 16777215));
        DirectoryPushButton->setAutoDefault(false);

        horizontalLayout_2->addWidget(DirectoryPushButton);


        verticalLayout->addWidget(groupBox_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        CreateCasePushButton = new QPushButton(NewCaseDialog);
        CreateCasePushButton->setObjectName(QStringLiteral("CreateCasePushButton"));

        horizontalLayout_3->addWidget(CreateCasePushButton);

        CancelPushButton = new QPushButton(NewCaseDialog);
        CancelPushButton->setObjectName(QStringLiteral("CancelPushButton"));
        CancelPushButton->setAutoDefault(false);

        horizontalLayout_3->addWidget(CancelPushButton);


        verticalLayout->addLayout(horizontalLayout_3);

        QWidget::setTabOrder(case_name_edit, CaseDescriptionLineEdit);
        QWidget::setTabOrder(CaseDescriptionLineEdit, TemplateComboBox);
        QWidget::setTabOrder(TemplateComboBox, DirectoryLineEdit);
        QWidget::setTabOrder(DirectoryLineEdit, DirectoryPushButton);
        QWidget::setTabOrder(DirectoryPushButton, CreateCasePushButton);
        QWidget::setTabOrder(CreateCasePushButton, CancelPushButton);

        retranslateUi(NewCaseDialog);
        QObject::connect(CreateCasePushButton, SIGNAL(clicked()), NewCaseDialog, SLOT(accept()));
        QObject::connect(CancelPushButton, SIGNAL(clicked()), NewCaseDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewCaseDialog);
    } // setupUi

    void retranslateUi(QDialog *NewCaseDialog)
    {
        NewCaseDialog->setWindowTitle(QApplication::translate("NewCaseDialog", "\320\235\320\276\320\262\320\260\321\217 \320\267\320\260\320\264\320\260\321\207\320\260", 0));
        CaseGroupBox->setTitle(QApplication::translate("NewCaseDialog", "\320\227\320\260\320\264\320\260\321\207\320\260", 0));
        CaseNameLabel->setText(QApplication::translate("NewCaseDialog", "\320\230\320\274\321\217:", 0));
        CaseDescriptionLabel->setText(QApplication::translate("NewCaseDialog", "\320\236\320\277\320\270\321\201\320\260\320\275\320\270\320\265:", 0));
        case_name_edit->setText(QApplication::translate("NewCaseDialog", "\320\235\320\276\320\262\320\260\321\217 \320\267\320\260\320\264\320\260\321\207\320\260", 0));
        groupBox->setTitle(QApplication::translate("NewCaseDialog", "\320\242\320\270\320\277 \320\270\320\263\321\200\320\276\320\262\320\276\320\263\320\276 \320\260\320\262\321\202\320\276\320\274\320\260\321\202\320\260", 0));
        TemplateComboBox->clear();
        TemplateComboBox->insertItems(0, QStringList()
         << QApplication::translate("NewCaseDialog", "\320\222\321\213\320\261\320\265\321\200\320\270\321\202\320\265 \321\202\320\270\320\277 \320\260\320\262\321\202\320\276\320\274\320\260\321\202\320\260", 0)
         << QApplication::translate("NewCaseDialog", "\320\220\320\262\321\202\320\276\320\274\320\260\321\202 1", 0)
         << QApplication::translate("NewCaseDialog", "\320\220\320\262\321\202\320\276\320\274\320\260\321\202 2", 0)
        );
        groupBox_2->setTitle(QApplication::translate("NewCaseDialog", "\320\232\320\260\321\202\320\260\320\273\320\276\320\263 \320\276\321\202\321\207\321\221\321\202\320\260", 0));
        DirectoryLineEdit->setText(QApplication::translate("NewCaseDialog", "\320\222\321\213\320\261\320\265\321\200\320\270\321\202\320\265 \320\272\320\260\321\202\320\260\320\273\320\276\320\263", 0));
        DirectoryPushButton->setText(QApplication::translate("NewCaseDialog", "...", 0));
        CreateCasePushButton->setText(QApplication::translate("NewCaseDialog", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214", 0));
        CancelPushButton->setText(QApplication::translate("NewCaseDialog", "\320\236\321\202\320\274\320\265\320\275\320\260", 0));
    } // retranslateUi

};

namespace Ui {
    class NewCaseDialog: public Ui_NewCaseDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWCASEDIALOG_H
