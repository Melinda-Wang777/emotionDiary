#include "diarywindow.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QImage>
#include <QTextCursor>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFontDatabase>

DiaryWindow::DiaryWindow(QWidget *parent)
    : BaseWindow(parent)
{
    m_isClosing = false;

    // ========== 加载汇文明朝体字体 ==========
    int fontId = QFontDatabase::addApplicationFont(":/resources/mingchao.otf");
    QString fontFamily;
    if (fontId != -1) {
        fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    } else {
        fontFamily = "KaiTi";  // 加载失败就用楷体
    }

    // 设置背景图
    setBackgroundImage(":/images/bg_diary.jpg");

    setWindowTitle("日记");
    setWindowFlags(Qt::Window);
    resize(650, 550);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setupToolBar();
    mainLayout->addWidget(m_toolBar);

    m_textEdit = new QTextEdit(this);
    m_textEdit->setPlaceholderText("在这里写下你的日记...");

    // ========== 设置默认字体 ==========
    QFont defaultFont = m_textEdit->font();
    defaultFont.setFamily(fontFamily);
    defaultFont.setPointSize(16);
    m_textEdit->setFont(defaultFont);
    m_fontCombo->setCurrentFont(defaultFont);

    mainLayout->addWidget(m_textEdit);

    // 编辑框样式
    m_textEdit->setStyleSheet(
        "QTextEdit {"
        "  background-color: transparent;"
        "  border: none;"

        "  padding: 20px;"
        "  font-size: 14px;"
        "}"
        "QTextEdit:focus {"
        "  background-color: rgba(255, 255, 255, 0.5);"
        "}"
        );

    // 工具栏样式
    m_toolBar->setStyleSheet(
        "QToolBar {"
        "  background: transparent;"
        "  border: none;"
        "  spacing: 6px;"
        "}"
        "QPushButton {"
        "  background-color: #2C2C2C;"
        "  color: #FDF9F5;"
        "  border: none;"
        "  border-radius: 12px;"
        "  padding: 6px 12px;"
        "  font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #4A4A4A;"
        "}"
        "QPushButton:checked {"
        "  background-color: #C4A882;"
        "  color: #2C2C2C;"
        "}"
        "QFontComboBox, QComboBox {"
        "  border: 1px solid #E0D8D0;"
        "  border-radius: 12px;"
        "  padding: 4px 8px;"
        "  background-color: white;"
        "  color: #5A4A3A;"
        "}"
        );

    connect(m_textEdit, &QTextEdit::cursorPositionChanged, this, [this]() {
        QTextCharFormat fmt = m_textEdit->currentCharFormat();
        m_boldBtn->setChecked(fmt.fontWeight() == QFont::Bold);
        m_italicBtn->setChecked(fmt.fontItalic());
        m_underlineBtn->setChecked(fmt.fontUnderline());
        m_strikeOutBtn->setChecked(fmt.fontStrikeOut());
    });

    connect(m_undoBtn, &QPushButton::clicked, m_textEdit, &QTextEdit::undo);
    connect(m_redoBtn, &QPushButton::clicked, m_textEdit, &QTextEdit::redo);
}

DiaryWindow::~DiaryWindow() {}

void DiaryWindow::closeEvent(QCloseEvent *event) {
    m_isClosing = true;
    saveDiary();
    event->accept();
}

void DiaryWindow::setupToolBar()
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);

    m_fontCombo = new QFontComboBox(this);
    m_fontCombo->setMaximumWidth(150);
    m_fontCombo->setStyleSheet("QFontComboBox QListView { padding: 3px; } QFontComboBox QListView::item { min-height: 14px; font-size: 7pt; }");
    m_toolBar->addWidget(m_fontCombo);

    m_fontSizeCombo = new QComboBox(this);
    m_fontSizeCombo->setMaximumWidth(60);
    m_fontSizeCombo->setEditable(true);
    QStringList sizes;
    sizes << "12" << "14" << "16" << "18" << "20" << "24" << "28";
    m_fontSizeCombo->addItems(sizes);
    m_fontSizeCombo->setCurrentText("16");
    m_toolBar->addWidget(m_fontSizeCombo);

    m_toolBar->addSeparator();

    m_boldBtn = new QPushButton("B", this);
    m_boldBtn->setCheckable(true);
    m_boldBtn->setFixedSize(30, 30);
    QFont boldFont = m_boldBtn->font();
    boldFont.setBold(true);
    m_boldBtn->setFont(boldFont);
    m_toolBar->addWidget(m_boldBtn);

    m_italicBtn = new QPushButton("I", this);
    m_italicBtn->setCheckable(true);
    m_italicBtn->setFixedSize(30, 30);
    QFont italicFont = m_italicBtn->font();
    italicFont.setItalic(true);
    m_italicBtn->setFont(italicFont);
    m_toolBar->addWidget(m_italicBtn);

    m_underlineBtn = new QPushButton("U", this);
    m_underlineBtn->setCheckable(true);
    m_underlineBtn->setFixedSize(30, 30);
    QFont underlineFont = m_underlineBtn->font();
    underlineFont.setUnderline(true);
    m_underlineBtn->setFont(underlineFont);
    m_toolBar->addWidget(m_underlineBtn);

    m_strikeOutBtn = new QPushButton("S", this);
    m_strikeOutBtn->setCheckable(true);
    m_strikeOutBtn->setFixedSize(30, 30);
    QFont strikeFont = m_strikeOutBtn->font();
    strikeFont.setStrikeOut(true);
    m_strikeOutBtn->setFont(strikeFont);
    m_toolBar->addWidget(m_strikeOutBtn);

    m_toolBar->addSeparator();

    m_imageBtn = new QPushButton("插入图片", this);
    m_toolBar->addWidget(m_imageBtn);
    //添加撤销/恢复按钮
    m_toolBar->addSeparator();

    m_undoBtn = new QPushButton("↩️ 撤销", this);
    m_redoBtn = new QPushButton("↪️ 恢复", this);
    m_toolBar->addWidget(m_undoBtn);
    m_toolBar->addWidget(m_redoBtn);

    connect(m_fontCombo, &QFontComboBox::currentFontChanged, this, &DiaryWindow::onFontChanged);
    connect(m_fontSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DiaryWindow::onFontSizeChanged);
    connect(m_boldBtn, &QPushButton::toggled, this, &DiaryWindow::onBoldToggled);
    connect(m_italicBtn, &QPushButton::toggled, this, &DiaryWindow::onItalicToggled);
    connect(m_underlineBtn, &QPushButton::toggled, this, &DiaryWindow::onUnderlineToggled);
    connect(m_strikeOutBtn, &QPushButton::toggled, this, &DiaryWindow::onStrikeOutToggled);
    connect(m_imageBtn, &QPushButton::clicked, this, &DiaryWindow::onInsertImage);
}

void DiaryWindow::mergeFormatOnSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = m_textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    m_textEdit->mergeCurrentCharFormat(format);
}

void DiaryWindow::onBoldToggled(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    mergeFormatOnSelection(fmt);
}

void DiaryWindow::onItalicToggled(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontItalic(checked);
    mergeFormatOnSelection(fmt);
}

void DiaryWindow::onUnderlineToggled(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(checked);
    mergeFormatOnSelection(fmt);
}

void DiaryWindow::onStrikeOutToggled(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(checked);
    mergeFormatOnSelection(fmt);
}

void DiaryWindow::onFontChanged(const QFont &font)
{
    QTextCharFormat fmt;
    fmt.setFontFamilies(QStringList() << font.family());
    mergeFormatOnSelection(fmt);
}

void DiaryWindow::onFontSizeChanged(int index)
{
    Q_UNUSED(index);
    bool ok;
    int size = m_fontSizeCombo->currentText().toInt(&ok);
    if (ok) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(size);
        mergeFormatOnSelection(fmt);
    }
}

void DiaryWindow::onInsertImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "选择图片", "",
        "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif)");

    if (filePath.isEmpty())
        return;

    QImage img(filePath);
    if (img.isNull()) {
        QMessageBox::warning(this, "错误", "无法加载图片");
        return;
    }

    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt = cursor.charFormat();//保存插入图片前的字符格式
    cursor.insertImage(img, filePath);
    cursor.setCharFormat(fmt);//恢复插入图片之前的字符格式
    m_textEdit->setTextCursor(cursor);
}

void DiaryWindow::loadDiary(const QString &date)
{
    m_currentDate = date;
    setWindowTitle(QString("日记 - %1").arg(date));

    QSqlDatabase &db = MainWindow::getDatabase();
    if (!db.isOpen()) {
        m_textEdit->clear();
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT diary FROM daily_emotions WHERE date = ?");
    query.addBindValue(date);

    if (query.exec() && query.next()) {
        QString html = query.value(0).toString();
        if (html.isEmpty())
            m_textEdit->clear();
        else
            m_textEdit->setHtml(html);
    } else {
        m_textEdit->clear();
    }
}

void DiaryWindow::saveDiary()
{
    if (m_currentDate.isEmpty())
        return;

    QSqlDatabase &db = MainWindow::getDatabase();
    if (!db.isOpen()) {
        return;
    }

    bool isEmpty = m_textEdit->document()->isEmpty();//需要判断日记是否为空

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT date FROM daily_emotions WHERE date = ?");
    checkQuery.addBindValue(m_currentDate);

    if (checkQuery.exec() && checkQuery.next()) {
        //有情绪记录
        if (isEmpty) {
            //日记为空，清空日记
            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE daily_emotions SET diary = NULL WHERE date = ?");
            updateQuery.addBindValue(m_currentDate);
            updateQuery.exec();
        } else {
            //日记不为空，保存
            QString html = m_textEdit->toHtml();
            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE daily_emotions SET diary = ? WHERE date = ?");
            updateQuery.addBindValue(html);
            updateQuery.addBindValue(m_currentDate);
            updateQuery.exec();
        }
    } else {
        //没有情绪记录
        if (isEmpty)
            return;

        QString html = m_textEdit->toHtml();
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO daily_emotions (date, p, a, d, diary) VALUES (?, 128, 128, 128, ?)");
        insertQuery.addBindValue(m_currentDate);
        insertQuery.addBindValue(html);
        insertQuery.exec();
    }
}
