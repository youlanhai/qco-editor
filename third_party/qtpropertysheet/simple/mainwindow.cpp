#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qttreepropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertyfactory.h"
#include "qtpropertyeditorfactory.h"
#include "qtattributename.h"
#include "qtbuttonpropertybrowser.h"

#include <QTreeWidget>
#include <QHBoxLayout>
#include <QDir>
#include <QScrollArea>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    root_(NULL)
{
    ui->setupUi(this);

    createProperties();

    popupMenu_ = new QMenu();
    popupMenu_->addAction(tr("Expand"));
    popupMenu_->addAction(tr("Remove"));


    QHBoxLayout *layout = new QHBoxLayout(ui->centralWidget);
    ui->centralWidget->setLayout(layout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    layout->addWidget(splitter);

    QtPropertyEditorFactory *editorFactory = new QtPropertyEditorFactory(this);

    {
        QScrollArea *scrollArea = new QScrollArea(ui->centralWidget);
        scrollArea->setWidgetResizable(true);
        splitter->addWidget(scrollArea);

        QWidget *widget = new QWidget();
        scrollArea->setWidget(widget);

        QtButtonPropertyBrowser *browser = new QtButtonPropertyBrowser(this);
        browser->init(widget, editorFactory);

        browser->addProperty(root_);
    }
    {
        QWidget *treeWidget = new QWidget(this);
        splitter->addWidget(treeWidget);

        QtTreePropertyBrowser *treeBrowser = new QtTreePropertyBrowser(this);
        treeBrowser->init(treeWidget, editorFactory);
        treeBrowser->addProperty(root_);
    }

    // test set property value

    //1. set child value by name.
    root_->setChildValue("name", "Jack");

    //2. find property, then set value directly.
    QtProperty *addressProperty = root_->findChild("age");
    if(addressProperty != NULL)
    {
        addressProperty->setValue(18);
    }

    //3. set list value
    QVariantList values;
    values.push_back(QVariant(8.0f));
    values.push_back(QVariant(9.0f));
    values.push_back(QVariant(200.0f));
    values.push_back(QVariant(100.0f));
    root_->setChildValue("geometry", values);


#if 0
    // test remove and add
    browser->removeProperty(root_);
    browser->addProperty(root_);
#endif

    //test delete
    // delete root
}

void MainWindow::createProperties()
{
    QtPropertyFactory *manager = new QtPropertyFactory(this);

    QtProperty *root = manager->createProperty(QtPropertyType::GROUP);
    root->setName("root");
    root->setSelfVisible(false);
    root_ = root;

    {
        QtProperty *group = manager->createProperty(QtPropertyType::GROUP);
        group->setBackgroundColor(Qt::darkGray);
        group->setTitle("information");
        group->setMenuVisible(true);
        connect(group, SIGNAL(signalPopupMenu(QtProperty*)), this, SLOT(onPopupMenu(QtProperty*)));

        QtProperty *property = manager->createProperty(QtPropertyType::STRING);
        property->setName("name");
        property->setValue(QString("no name"));
        group->addChild(property);

        QtProperty *property2 = manager->createProperty(QtPropertyType::INT);
        property2->setName("age");
        group->addChild(property2);

        QtProperty *property3 = manager->createProperty(QtPropertyType::FLOAT);
        property3->setName("weight");
        property3->setValue(60.5);
        group->addChild(property3);

        QtProperty *property4 = manager->createProperty(QtPropertyType::ENUM);
        property4->setName("country");
        QStringList countries;
        countries << "China" << "America" << "England";
        property4->setAttribute("enumNames", countries);
        group->addChild(property4);

        QtProperty *property5 = manager->createProperty(QtPropertyType::BOOL);
        property5->setName("married");
        property5->setValue(true);
        group->addChild(property5);

        QtProperty *property6 = manager->createProperty(QtPropertyType::COLOR);
        property6->setName("color");
        property6->setValue(QColor(255, 0, 0));
        group->addChild(property6);

        QtProperty *property7 = manager->createProperty(QtPropertyType::FLAG);
        property7->setName("hobby");
        property7->setValue(1);
        QStringList hobby;
        hobby << "running" << "walking" << "swimming";
        property7->setAttribute("flagNames", hobby);
        group->addChild(property7);

        QtProperty *property8 = manager->createProperty(QtPropertyType::FILE);
        property8->setName("head icon");
        property8->setValue(QString("no-image"));
        property8->setAttribute(QtAttributeName::FileDialogFilter, QString("Images(*.png *.jpg)"));
        property8->setAttribute(QtAttributeName::FileRelativePath, QDir::currentPath());
        group->addChild(property8);

        QtProperty *property9 = manager->createProperty(QtPropertyType::ENUM_PAIR);
        property9->setName("sex");
        QStringList titles;
        titles << "unkown" << "man" << "woman";
        property9->setAttribute(QtAttributeName::EnumName, titles);
        property9->setAttribute(QtAttributeName::EnumValues, titles);
        property9->setValue("unkown");
        group->addChild(property9);

        QtProperty *property10 = manager->createProperty(QtPropertyType::FLOAT_LIST);
        property10->setName("position");
        property10->setAttribute(QtAttributeName::Size, 3);
        property10->setAttribute(QtAttributeName::MinValue, -1000.0f);
        property10->setAttribute(QtAttributeName::MaxValue, 1000.0f);
        group->addChild(property10);

        root->addChild(group);
    }
    {
        QtProperty *hideRect = manager->createProperty(QtPropertyType::BOOL);
        hideRect->setName("show geometry");
        hideRect->setValue(true);
        root->addChild(hideRect);

        QtProperty *rect = manager->createProperty(QtPropertyType::LIST);
        rect->setName("geometry");

        QtProperty *x = manager->createProperty(QtPropertyType::FLOAT);
        x->setName("x");
        rect->addChild(x);

        QtProperty *y = manager->createProperty(QtPropertyType::FLOAT);
        y->setName("y");
        rect->addChild(y);

        QtProperty *width = manager->createProperty(QtPropertyType::FLOAT);
        width->setName("width");
        rect->addChild(width);

        QtProperty *height = manager->createProperty(QtPropertyType::FLOAT);
        height->setName("height");
        rect->addChild(height);

        root->addChild(rect);
    }
    {
        QtProperty *prop = manager->createProperty(QtPropertyType::DYNAMIC_LIST);
        prop->setName("dynamic list");
        prop->setAttribute("valueType", QtPropertyType::FILE);

        QVariantList values;
        values.push_back(QString("name"));
        values.push_back(QString("age"));
        prop->setValue(values);
        root->addChild(prop);
    }

    connect(root, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onValueChanged(QtProperty*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onValueChanged(QtProperty *property)
{
    printf("property change: %s = %s\n",
           property->getName().toUtf8().data(),
           property->getValueString().toUtf8().data());

    fflush(stdout);

    if(property->getName() == "show geometry")
    {
        QtProperty *geometry = root_->findChild("geometry");
        if(geometry != NULL)
        {
            geometry->setVisible(property->getValue().toBool());
        }
    }
}

void MainWindow::onPopupMenu(QtProperty *property)
{
    popupMenu_->popup(QCursor::pos());
}
