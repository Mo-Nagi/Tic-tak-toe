#include "TicTacToeGUI.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

TicTacToeGUI::TicTacToeGUI(QWidget *parent)
    : QMainWindow(parent)
    , stackedWidget(nullptr)
    , menuWidget(nullptr)
    , loginWidget(nullptr)
    , registerWidget(nullptr)
    , gameModeWidget(nullptr)
    , symbolSelectionWidget(nullptr)
    , difficultySelectionWidget(nullptr)
    , gameWidget(nullptr)
    , historyWidget(nullptr)
    , replayWidget(nullptr)
    , settingsWidget(nullptr)
    , autoPlayTimer(nullptr)
    , notificationTimer(nullptr)
    , notificationLabel(nullptr)
    , currentState(GameState::MENU)
    , currentGameMode(GameMode::PLAYER_VS_PLAYER)
    , currentPlayer('X')
    , playerSymbol('X')
    , secondPlayerSymbol('O')
    , gameEnded(false)
    , currentReplayMoveIndex(0)
    , isAutoPlaying(false)
    , currentTheme(ThemeMode::DARK)
    , currentDifficulty(DifficultyLevel::HARD)
{
    try {
        gameBoard = std::make_unique<GameBoard>();
        userManager = nullptr;
        aiPlayer = nullptr;
        gameHistory = nullptr;
        gameStateStack = std::make_unique<GameStateStack>();

        loadThemeSettings();
        setupUI();
        setupNotificationSystem();
        applyTheme();

        setWindowTitle("Tic Tac Toe Game");
        // حجم مثالي للتصميم الجديد
        setMinimumSize(750, 620);
        setMaximumSize(750, 620);
        resize(750, 620);
        setFixedSize(750, 620);

    } catch (const std::exception& e) {
        qDebug() << "Constructor error:" << e.what();
        throw;
    }
}

TicTacToeGUI::~TicTacToeGUI() {
    saveThemeSettings();
}

void TicTacToeGUI::setupNotificationSystem() {
    notificationLabel = new QLabel(this);
    notificationLabel->setObjectName("notificationLabel");
    notificationLabel->setAlignment(Qt::AlignCenter);
    notificationLabel->setWordWrap(true);
    notificationLabel->hide();

    notificationTimer = new QTimer(this);
    notificationTimer->setSingleShot(true);
    connect(notificationTimer, &QTimer::timeout, this, &TicTacToeGUI::hideNotification);
}

void TicTacToeGUI::showGameNotification(const QString& message, const QString& type) {
    if (!notificationLabel) return;

    notificationLabel->setText(message);
    notificationLabel->setProperty("notificationType", type);

    // تحديد موضع النافذة المنبثقة في وسط الشاشة
    int width = 300;
    int height = 100;
    int x = (this->width() - width) / 2;
    int y = (this->height() - height) / 2;

    notificationLabel->setGeometry(x, y, width, height);
    notificationLabel->show();
    notificationLabel->raise();

    // إخفاء النافذة بعد 3 ثوانٍ
    notificationTimer->start(3000);
}

void TicTacToeGUI::hideNotification() {
    if (notificationLabel) {
        notificationLabel->hide();
    }
}

void TicTacToeGUI::setupUI() {
    stackedWidget = new QStackedWidget();
    setCentralWidget(stackedWidget);

    setupMenuScreen();
    setupLoginScreen();
    setupRegisterScreen();
    setupGameModeScreen();
    setupSymbolSelectionScreen();
    setupDifficultySelectionScreen();
    setupGameScreen();
    setupHistoryScreen();
    setupReplayScreen();
    setupSettingsScreen();

    switchToScreen(GameState::MENU);
}

void TicTacToeGUI::setupMenuScreen() {
    menuWidget = new QWidget();

    QVBoxLayout* mainLayout = new QVBoxLayout(menuWidget);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // مساحة فارغة علوية أصغر
    mainLayout->addStretch(1);

    QVBoxLayout* centerLayout = new QVBoxLayout();
    centerLayout->setAlignment(Qt::AlignCenter);
    centerLayout->setSpacing(25);

    titleLabel = new QLabel("🎮 لعبة تيك تاك تو");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");

    loginButton = new QPushButton("🔑 تسجيل الدخول");
    registerButton = new QPushButton("📝 إنشاء حساب جديد");

    loginButton->setObjectName("primaryButton");
    registerButton->setObjectName("secondaryButton");

    loginButton->setFixedSize(220, 45);
    registerButton->setFixedSize(220, 45);

    centerLayout->addWidget(titleLabel);
    centerLayout->addSpacing(15);
    centerLayout->addWidget(loginButton);
    centerLayout->addWidget(registerButton);

    mainLayout->addLayout(centerLayout);
    mainLayout->addStretch(1);

    // تعديل bottomLayout ليكون الزر في أقصى اليمين فقط
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    settingsButton = new QPushButton("⚙️");
    settingsButton->setObjectName("settingsButtonSmall");
    settingsButton->setFixedSize(40, 40);
    settingsButton->setToolTip("الإعدادات");

    bottomLayout->addWidget(settingsButton);
    bottomLayout->setContentsMargins(0, 0, 15, 15);

    mainLayout->addLayout(bottomLayout);

    connect(loginButton, &QPushButton::clicked, this, &TicTacToeGUI::onShowLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &TicTacToeGUI::onShowRegisterClicked);
    connect(settingsButton, &QPushButton::clicked, this, &TicTacToeGUI::onSettingsClicked);

    stackedWidget->addWidget(menuWidget);
}

void TicTacToeGUI::setupLoginScreen() {
    loginWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(loginWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(15);

    QLabel* titleLabel = new QLabel("🔑 تسجيل الدخول");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");

    loginUsernameEdit = new QLineEdit();
    loginUsernameEdit->setPlaceholderText("اسم المستخدم");
    loginUsernameEdit->setObjectName("inputField");

    loginPasswordEdit = new QLineEdit();
    loginPasswordEdit->setPlaceholderText("كلمة المرور");
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginPasswordEdit->setObjectName("inputField");

    loginSubmitButton = new QPushButton("دخول");
    loginSubmitButton->setObjectName("primaryButton");

    showRegisterButton = new QPushButton("ليس لديك حساب؟ أنشئ حساباً جديداً");
    showRegisterButton->setObjectName("linkButton");

    loginBackButton = new QPushButton("⬅️ رجوع");
    loginBackButton->setObjectName("backButton");

    layout->addWidget(titleLabel);
    layout->addWidget(loginUsernameEdit);
    layout->addWidget(loginPasswordEdit);
    layout->addWidget(loginSubmitButton);
    layout->addWidget(showRegisterButton);
    layout->addWidget(loginBackButton);

    connect(loginSubmitButton, &QPushButton::clicked, this, &TicTacToeGUI::onLoginClicked);
    connect(showRegisterButton, &QPushButton::clicked, this, &TicTacToeGUI::onShowRegisterClicked);
    connect(loginBackButton, &QPushButton::clicked, this, &TicTacToeGUI::onBackToMenuClicked);

    stackedWidget->addWidget(loginWidget);
}

void TicTacToeGUI::setupRegisterScreen() {
    registerWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(registerWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(15);

    QLabel* titleLabel = new QLabel("📝 إنشاء حساب جديد");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");

    registerUsernameEdit = new QLineEdit();
    registerUsernameEdit->setPlaceholderText("اسم المستخدم");
    registerUsernameEdit->setObjectName("inputField");

    registerPasswordEdit = new QLineEdit();
    registerPasswordEdit->setPlaceholderText("كلمة المرور");
    registerPasswordEdit->setEchoMode(QLineEdit::Password);
    registerPasswordEdit->setObjectName("inputField");

    registerConfirmPasswordEdit = new QLineEdit();
    registerConfirmPasswordEdit->setPlaceholderText("تأكيد كلمة المرور");
    registerConfirmPasswordEdit->setEchoMode(QLineEdit::Password);
    registerConfirmPasswordEdit->setObjectName("inputField");

    registerSubmitButton = new QPushButton("إنشاء الحساب");
    registerSubmitButton->setObjectName("primaryButton");

    showLoginButton = new QPushButton("لديك حساب؟ سجل دخولك");
    showLoginButton->setObjectName("linkButton");

    registerBackButton = new QPushButton("⬅️ رجوع");
    registerBackButton->setObjectName("backButton");

    layout->addWidget(titleLabel);
    layout->addWidget(registerUsernameEdit);
    layout->addWidget(registerPasswordEdit);
    layout->addWidget(registerConfirmPasswordEdit);
    layout->addWidget(registerSubmitButton);
    layout->addWidget(showLoginButton);
    layout->addWidget(registerBackButton);

    connect(registerSubmitButton, &QPushButton::clicked, this, &TicTacToeGUI::onRegisterClicked);
    connect(showLoginButton, &QPushButton::clicked, this, &TicTacToeGUI::onShowLoginClicked);
    connect(registerBackButton, &QPushButton::clicked, this, &TicTacToeGUI::onBackToMenuClicked);

    stackedWidget->addWidget(registerWidget);
}

void TicTacToeGUI::setupGameModeScreen() {
    gameModeWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(gameModeWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    welcomeLabel = new QLabel();
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setObjectName("titleLabel");

    statsLabel = new QLabel();
    statsLabel->setAlignment(Qt::AlignCenter);
    statsLabel->setObjectName("statsLabel");

    playerVsPlayerButton = new QPushButton("👥 لاعب ضد لاعب");
    playerVsAIButton = new QPushButton("🤖 لاعب ضد الكمبيوتر");
    gameHistoryButton = new QPushButton("📊 تاريخ الألعاب");
    logoutButton = new QPushButton("🚪 تسجيل الخروج");

    playerVsPlayerButton->setObjectName("primaryButton");
    playerVsAIButton->setObjectName("primaryButton");
    gameHistoryButton->setObjectName("secondaryButton");
    logoutButton->setObjectName("backButton");

    layout->addWidget(welcomeLabel);
    layout->addWidget(statsLabel);
    layout->addWidget(playerVsPlayerButton);
    layout->addWidget(playerVsAIButton);
    layout->addWidget(gameHistoryButton);
    layout->addWidget(logoutButton);

    connect(playerVsPlayerButton, &QPushButton::clicked, this, &TicTacToeGUI::onPlayerVsPlayerClicked);
    connect(playerVsAIButton, &QPushButton::clicked, this, &TicTacToeGUI::onPlayerVsAIClicked);
    connect(gameHistoryButton, &QPushButton::clicked, this, &TicTacToeGUI::onGameHistoryClicked);
    connect(logoutButton, &QPushButton::clicked, this, &TicTacToeGUI::onLogoutClicked);

    stackedWidget->addWidget(gameModeWidget);
}

void TicTacToeGUI::setupSymbolSelectionScreen() {
    symbolSelectionWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(symbolSelectionWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    symbolSelectionLabel = new QLabel("اختر رمزك");
    symbolSelectionLabel->setAlignment(Qt::AlignCenter);
    symbolSelectionLabel->setObjectName("titleLabel");

    symbolButtonGroup = new QButtonGroup(this);
    symbolXRadio = new QRadioButton("❌ اختر الرمز");
    symbolORadio = new QRadioButton("⭕ اختر الرمز");
    symbolXRadio->setChecked(true);

    symbolButtonGroup->addButton(symbolXRadio);
    symbolButtonGroup->addButton(symbolORadio);

    symbolConfirmButton = new QPushButton("تأكيد");
    symbolConfirmButton->setObjectName("primaryButton");

    symbolBackButton = new QPushButton("⬅️ رجوع");
    symbolBackButton->setObjectName("backButton");

    layout->addWidget(symbolSelectionLabel);
    layout->addWidget(symbolXRadio);
    layout->addWidget(symbolORadio);
    layout->addWidget(symbolConfirmButton);
    layout->addWidget(symbolBackButton);

    connect(symbolConfirmButton, &QPushButton::clicked, this, &TicTacToeGUI::onSymbolSelectionClicked);
    connect(symbolBackButton, &QPushButton::clicked, this, &TicTacToeGUI::onBackFromSymbolSelectionClicked);

    stackedWidget->addWidget(symbolSelectionWidget);
}

void TicTacToeGUI::setupDifficultySelectionScreen() {
    difficultySelectionWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(difficultySelectionWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    difficultySelectionLabel = new QLabel("اختر مستوى الصعوبة");
    difficultySelectionLabel->setAlignment(Qt::AlignCenter);
    difficultySelectionLabel->setObjectName("titleLabel");

    difficultyButtonGroup = new QButtonGroup(this);
    easyRadio = new QRadioButton("😊 سهل");
    mediumRadio = new QRadioButton("😐 متوسط");
    hardRadio = new QRadioButton("😈 صعب");
    hardRadio->setChecked(true);

    difficultyButtonGroup->addButton(easyRadio);
    difficultyButtonGroup->addButton(mediumRadio);
    difficultyButtonGroup->addButton(hardRadio);

    difficultyConfirmButton = new QPushButton("ابدأ اللعب");
    difficultyConfirmButton->setObjectName("primaryButton");

    difficultyBackButton = new QPushButton("⬅️ رجوع");
    difficultyBackButton->setObjectName("backButton");

    layout->addWidget(difficultySelectionLabel);
    layout->addWidget(easyRadio);
    layout->addWidget(mediumRadio);
    layout->addWidget(hardRadio);
    layout->addWidget(difficultyConfirmButton);
    layout->addWidget(difficultyBackButton);

    connect(difficultyConfirmButton, &QPushButton::clicked, this, &TicTacToeGUI::onDifficultySelectionClicked);
    connect(difficultyBackButton, &QPushButton::clicked, this, &TicTacToeGUI::onBackFromDifficultySelectionClicked);

    stackedWidget->addWidget(difficultySelectionWidget);
}

void TicTacToeGUI::setupGameScreen() {
    try {
        gameWidget = new QWidget();

        // Layout رئيسي يملأ النافذة بالكامل
        QVBoxLayout* mainLayout = new QVBoxLayout(gameWidget);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(0);

        // ========== منطقة العنوان (أعلى الشاشة) ==========
        QWidget* titleSection = new QWidget();
        titleSection->setFixedHeight(80);
        QVBoxLayout* titleLayout = new QVBoxLayout(titleSection);
        titleLayout->setAlignment(Qt::AlignCenter);
        titleLayout->setContentsMargins(0, 10, 0, 10);

        gameStatusLabel = new QLabel("🎮 جاري اللعب");
        gameStatusLabel->setAlignment(Qt::AlignCenter);
        gameStatusLabel->setObjectName("titleLabel");

        currentPlayerLabel = new QLabel();
        currentPlayerLabel->setAlignment(Qt::AlignCenter);
        currentPlayerLabel->setObjectName("statusLabel");

        titleLayout->addWidget(gameStatusLabel);
        titleLayout->addWidget(currentPlayerLabel);

        // ========== مساحة فارغة مرنة قبل المربعات ==========
        QWidget* topSpacer = new QWidget();
        topSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // ========== منطقة اللعب (وسط الشاشة تماماً) ==========
        QWidget* gameSection = new QWidget();
        gameSection->setFixedHeight(280);
        QVBoxLayout* gameSectionLayout = new QVBoxLayout(gameSection);
        gameSectionLayout->setAlignment(Qt::AlignCenter);
        gameSectionLayout->setContentsMargins(0, 0, 0, 0);

        // الشبكة
        QGridLayout* gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(0, 0, 0, 0);

        gameButtons.resize(3);
        for (int i = 0; i < 3; i++) {
            gameButtons[i].resize(3);
            for (int j = 0; j < 3; j++) {
                gameButtons[i][j] = new QPushButton();
                gameButtons[i][j]->setFixedSize(85, 85);
                gameButtons[i][j]->setObjectName("gameCell");
                connect(gameButtons[i][j], &QPushButton::clicked, this, &TicTacToeGUI::onCellClicked);
                gridLayout->addWidget(gameButtons[i][j], i, j);
            }
        }

        QWidget* gridWidget = new QWidget();
        gridWidget->setLayout(gridLayout);
        gridWidget->setFixedSize(270, 270);

        gameSectionLayout->addWidget(gridWidget);

        // ========== مساحة فارغة مرنة بعد المربعات ==========
        QWidget* bottomSpacer = new QWidget();
        bottomSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // ========== منطقة الأزرار (أسفل الشاشة - صف واحد أفقي) ==========
        QWidget* buttonSection = new QWidget();
        buttonSection->setFixedHeight(70);
        QVBoxLayout* buttonSectionLayout = new QVBoxLayout(buttonSection);
        buttonSectionLayout->setAlignment(Qt::AlignCenter);
        buttonSectionLayout->setContentsMargins(0, 15, 0, 15);

        // جميع الأزرار في صف واحد أفقي
        QHBoxLayout* allButtonsLayout = new QHBoxLayout();
        allButtonsLayout->setAlignment(Qt::AlignCenter);
        allButtonsLayout->setSpacing(20);

        undoButton = new QPushButton("↶ ارجع حركة للخلف");
        newGameButton = new QPushButton("🔄 لعبة جديدة");
        backToModeButton = new QPushButton("⬅️ رجوع للقائمة السابقة");

        undoButton->setObjectName("secondaryButton");
        newGameButton->setObjectName("primaryButton");
        backToModeButton->setObjectName("backButton");

        // أحجام متناسقة للأزرار
        undoButton->setFixedSize(110, 40);
        newGameButton->setFixedSize(140, 40);
        backToModeButton->setFixedSize(180, 40);

        allButtonsLayout->addWidget(undoButton);
        allButtonsLayout->addWidget(newGameButton);
        allButtonsLayout->addWidget(backToModeButton);

        buttonSectionLayout->addLayout(allButtonsLayout);

        // ========== تجميع جميع الأقسام مع المسافات المرنة ==========
        mainLayout->addWidget(titleSection);        // العنوان في الأعلى
        mainLayout->addWidget(topSpacer);           // مساحة مرنة
        mainLayout->addWidget(gameSection);         // المربعات في المنتصف
        mainLayout->addWidget(bottomSpacer);        // مساحة مرنة
        mainLayout->addWidget(buttonSection);       // الأزرار في الأسفل

        connect(newGameButton, &QPushButton::clicked, this, &TicTacToeGUI::onNewGameClicked);
        connect(undoButton, &QPushButton::clicked, this, &TicTacToeGUI::onUndoMoveClicked);
        connect(backToModeButton, &QPushButton::clicked, [this]() {
            switchToScreen(GameState::GAME_MODE_SELECTION);
        });

        stackedWidget->addWidget(gameWidget);
    } catch (const std::exception& e) {
        qDebug() << "Error in setupGameScreen:" << e.what();
        throw;
    }
}

void TicTacToeGUI::setupHistoryScreen() {
    historyWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(historyWidget);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(15);

    QLabel* titleLabel = new QLabel("📊 تاريخ الألعاب");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLabel");

    // تقسيم الشاشة أفقياً
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15);

    historyList = new QListWidget();
    historyList->setObjectName("historyList");
    historyList->setFixedWidth(300);
    historyList->setMinimumHeight(400);

    gameDetailsText = new QTextEdit();
    gameDetailsText->setReadOnly(true);
    gameDetailsText->setObjectName("gameDetails");
    gameDetailsText->setMinimumHeight(400);

    contentLayout->addWidget(historyList);
    contentLayout->addWidget(gameDetailsText);

    // أزرار التحكم
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(12);

    replayGameButton = new QPushButton("▶️ إعادة تشغيل اللعبة");
    replayGameButton->setObjectName("primaryButton");
    replayGameButton->setEnabled(false);
    replayGameButton->setFixedSize(180, 35);

    historyBackButton = new QPushButton("⬅️ رجوع");
    historyBackButton->setObjectName("backButton");
    historyBackButton->setFixedSize(100, 32);

    buttonsLayout->addWidget(replayGameButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(historyBackButton);

    layout->addWidget(titleLabel);
    layout->addLayout(contentLayout);
    layout->addSpacing(10);
    layout->addLayout(buttonsLayout);

    connect(historyList, &QListWidget::itemClicked, [this](QListWidgetItem* item) {
        int index = historyList->row(item);
        if (gameHistory) {
            auto userGames = gameHistory->getUserGames(currentUser);
            if (index >= 0 && index < userGames.size()) {
                const auto& record = userGames[index];
                QString details = QString("اللاعب الأول: %1\nاللاعب الثاني: %2\nالنتيجة: %3\nالوقت: %4")
                                      .arg(QString::fromStdString(record.player1))
                                      .arg(QString::fromStdString(record.player2))
                                      .arg(record.result == GameResult::PLAYER1_WIN ? "فوز اللاعب الأول" :
                                               record.result == GameResult::PLAYER2_WIN ? "فوز اللاعب الثاني" :
                                               record.result == GameResult::TIE ? "تعادل" : "جاري")
                                      .arg(QString::fromStdString(record.timestamp));
                gameDetailsText->setText(details);
                replayGameButton->setEnabled(true);
            }
        }
    });

    connect(replayGameButton, &QPushButton::clicked, this, &TicTacToeGUI::onReplayGameClicked);
    connect(historyBackButton, &QPushButton::clicked, [this]() {
        switchToScreen(GameState::GAME_MODE_SELECTION);
    });

    stackedWidget->addWidget(historyWidget);
}

void TicTacToeGUI::setupReplayScreen() {
    replayWidget = new QWidget();

    // Layout رئيسي يملأ النافذة
    QVBoxLayout* mainLayout = new QVBoxLayout(replayWidget);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(0);

    // ========== منطقة العناوين (أعلى الشاشة) ==========
    QWidget* titleSection = new QWidget();
    titleSection->setFixedHeight(90);
    QVBoxLayout* titleLayout = new QVBoxLayout(titleSection);
    titleLayout->setAlignment(Qt::AlignCenter);
    titleLayout->setSpacing(5);
    titleLayout->setContentsMargins(0, 5, 0, 5);

    replayStatusLabel = new QLabel("📺 إعادة تشغيل اللعبة");
    replayStatusLabel->setAlignment(Qt::AlignCenter);
    replayStatusLabel->setObjectName("titleLabel");

    replayMoveLabel = new QLabel();
    replayMoveLabel->setAlignment(Qt::AlignCenter);
    replayMoveLabel->setObjectName("statusLabel");

    replayInfoLabel = new QLabel();
    replayInfoLabel->setAlignment(Qt::AlignCenter);
    replayInfoLabel->setObjectName("infoLabel");

    titleLayout->addWidget(replayStatusLabel);
    titleLayout->addWidget(replayMoveLabel);
    titleLayout->addWidget(replayInfoLabel);

    // ========== مساحة فارغة مرنة قبل المربعات ==========
    QWidget* topSpacer = new QWidget();
    topSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ========== منطقة اللعب (وسط الشاشة) ==========
    QWidget* gameSection = new QWidget();
    gameSection->setFixedHeight(280);
    QVBoxLayout* gameSectionLayout = new QVBoxLayout(gameSection);
    gameSectionLayout->setAlignment(Qt::AlignCenter);
    gameSectionLayout->setContentsMargins(0, 0, 0, 0);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    replayButtons.resize(3);
    for (int i = 0; i < 3; i++) {
        replayButtons[i].resize(3);
        for (int j = 0; j < 3; j++) {
            replayButtons[i][j] = new QPushButton();
            replayButtons[i][j]->setFixedSize(85, 85);
            replayButtons[i][j]->setObjectName("gameCell");
            replayButtons[i][j]->setEnabled(false);
            gridLayout->addWidget(replayButtons[i][j], i, j);
        }
    }

    QWidget* gridWidget = new QWidget();
    gridWidget->setLayout(gridLayout);
    gridWidget->setFixedSize(270, 270);

    gameSectionLayout->addWidget(gridWidget);

    // ========== مساحة فارغة مرنة بعد المربعات ==========
    QWidget* bottomSpacer = new QWidget();
    bottomSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ========== منطقة الأزرار (أسفل الشاشة - صفين منظمين) ==========
    QWidget* buttonSection = new QWidget();
    buttonSection->setFixedHeight(90);
    QVBoxLayout* buttonSectionLayout = new QVBoxLayout(buttonSection);
    buttonSectionLayout->setAlignment(Qt::AlignCenter);
    buttonSectionLayout->setSpacing(12);
    buttonSectionLayout->setContentsMargins(0, 10, 0, 10);

    // الصف الأول: أزرار التحكم في الإعادة
    QHBoxLayout* replayControlsLayout = new QHBoxLayout();
    replayControlsLayout->setAlignment(Qt::AlignCenter);
    replayControlsLayout->setSpacing(15);

    replayPrevButton = new QPushButton("⏮️ السابق");
    replayNextButton = new QPushButton("⏭️ التالي");
    replayAutoPlayButton = new QPushButton("⏯️ تشغيل تلقائي");

    replayPrevButton->setObjectName("secondaryButton");
    replayNextButton->setObjectName("secondaryButton");
    replayAutoPlayButton->setObjectName("primaryButton");

    replayPrevButton->setFixedSize(100, 38);
    replayNextButton->setFixedSize(100, 38);
    replayAutoPlayButton->setFixedSize(140, 38);

    replayControlsLayout->addWidget(replayPrevButton);
    replayControlsLayout->addWidget(replayNextButton);
    replayControlsLayout->addWidget(replayAutoPlayButton);

    // الصف الثاني: زر الرجوع
    QHBoxLayout* backButtonLayout = new QHBoxLayout();
    backButtonLayout->setAlignment(Qt::AlignCenter);

    replayBackButton = new QPushButton("⬅️ رجوع للتاريخ");
    replayBackButton->setObjectName("backButton");
    replayBackButton->setFixedSize(160, 35);

    backButtonLayout->addWidget(replayBackButton);

    buttonSectionLayout->addLayout(replayControlsLayout);
    buttonSectionLayout->addLayout(backButtonLayout);

    // ========== تجميع الأقسام مع المسافات المرنة ==========
    mainLayout->addWidget(titleSection);       // العناوين في الأعلى
    mainLayout->addWidget(topSpacer);          // مساحة مرنة
    mainLayout->addWidget(gameSection);        // المربعات في المنتصف
    mainLayout->addWidget(bottomSpacer);       // مساحة مرنة
    mainLayout->addWidget(buttonSection);      // الأزرار في الأسفل

    connect(replayPrevButton, &QPushButton::clicked, this, &TicTacToeGUI::onReplayPrevClicked);
    connect(replayNextButton, &QPushButton::clicked, this, &TicTacToeGUI::onReplayNextClicked);
    connect(replayAutoPlayButton, &QPushButton::clicked, this, &TicTacToeGUI::onReplayAutoPlayClicked);
    connect(replayBackButton, &QPushButton::clicked, this, &TicTacToeGUI::onReplayBackClicked);

    stackedWidget->addWidget(replayWidget);
}

void TicTacToeGUI::setupSettingsScreen() {
    settingsWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(settingsWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    settingsLabel = new QLabel("⚙️ الإعدادات");
    settingsLabel->setAlignment(Qt::AlignCenter);
    settingsLabel->setObjectName("titleLabel");

    QLabel* themeLabel = new QLabel("المظهر:");
    themeLabel->setObjectName("settingsItemLabel");

    themeComboBox = new QComboBox();
    themeComboBox->addItem("فاتح", static_cast<int>(ThemeMode::LIGHT));
    themeComboBox->addItem("داكن", static_cast<int>(ThemeMode::DARK));
    themeComboBox->setObjectName("settingsComboBox");

    // تحديد الوضع الحالي بشكل صحيح
    themeComboBox->setCurrentIndex(static_cast<int>(currentTheme));

    settingsBackButton = new QPushButton("⬅️ رجوع");
    settingsBackButton->setObjectName("backButton");

    layout->addWidget(settingsLabel);
    layout->addWidget(themeLabel);
    layout->addWidget(themeComboBox);
    layout->addWidget(settingsBackButton);

    connect(themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TicTacToeGUI::onThemeChanged);
    connect(settingsBackButton, &QPushButton::clicked, this, &TicTacToeGUI::onBackFromSettingsClicked);

    stackedWidget->addWidget(settingsWidget);
}

void TicTacToeGUI::switchToScreen(GameState state) {
    currentState = state;

    switch (state) {
    case GameState::MENU:
        stackedWidget->setCurrentWidget(menuWidget);
        // إزالة تغيير الحجم - النافذة ثابتة
        break;
    case GameState::LOGIN:
        stackedWidget->setCurrentWidget(loginWidget);
        if (loginUsernameEdit) loginUsernameEdit->clear();
        if (loginPasswordEdit) loginPasswordEdit->clear();
        break;
    case GameState::REGISTER:
        stackedWidget->setCurrentWidget(registerWidget);
        if (registerUsernameEdit) registerUsernameEdit->clear();
        if (registerPasswordEdit) registerPasswordEdit->clear();
        if (registerConfirmPasswordEdit) registerConfirmPasswordEdit->clear();
        break;
    case GameState::GAME_MODE_SELECTION:
        stackedWidget->setCurrentWidget(gameModeWidget);
        updateGameModeDisplay();
        break;
    case GameState::SYMBOL_SELECTION:
        stackedWidget->setCurrentWidget(symbolSelectionWidget);
        break;
    case GameState::DIFFICULTY_SELECTION:
        stackedWidget->setCurrentWidget(difficultySelectionWidget);
        break;
    case GameState::PLAYING:
        stackedWidget->setCurrentWidget(gameWidget);
        break;
    case GameState::GAME_HISTORY:
        stackedWidget->setCurrentWidget(historyWidget);
        updateHistoryDisplay();
        break;
    case GameState::REPLAY_MODE:
        stackedWidget->setCurrentWidget(replayWidget);
        break;
    case GameState::SETTINGS:
        stackedWidget->setCurrentWidget(settingsWidget);
        if (themeComboBox) {
            themeComboBox->blockSignals(true);
            themeComboBox->setCurrentIndex(static_cast<int>(currentTheme));
            themeComboBox->blockSignals(false);
        }
        break;
    }
}

void TicTacToeGUI::updateGameModeDisplay() {
    if (welcomeLabel && !currentUser.empty()) {
        welcomeLabel->setText(QString("مرحباً، %1! 👋").arg(QString::fromStdString(currentUser)));
    }

    if (statsLabel && userManager) {
        User* user = userManager->getUser(currentUser);
        if (user) {
            QString stats = QString("الألعاب: %1 | الانتصارات: %2 | الهزائم: %3 | التعادل: %4")
                                .arg(user->gamesPlayed)
                                .arg(user->gamesWon)
                                .arg(user->gamesLost)
                                .arg(user->gamesTied);
            statsLabel->setText(stats);
        }
    }
}

void TicTacToeGUI::resetGame() {
    try {
        if (!gameBoard) gameBoard = std::make_unique<GameBoard>();
        gameBoard->reset();

        if (gameStateStack) {
            gameStateStack->clearStack();
        }

        currentPlayer = playerSymbol;
        gameEnded = false;
        currentGameMoves.clear();

        if (!gameWidget) {
            setupGameScreen();
        }

        if (gameButtons.size() == 3) {
            for (int i = 0; i < 3; i++) {
                if (gameButtons[i].size() == 3) {
                    for (int j = 0; j < 3; j++) {
                        if (gameButtons[i][j]) {
                            gameButtons[i][j]->setText("");
                            gameButtons[i][j]->setEnabled(true);
                            gameButtons[i][j]->setStyleSheet("");
                        }
                    }
                }
            }
        }

        if (currentGameMode == GameMode::PLAYER_VS_AI && !aiPlayer) {
            initializeAIPlayer();
        }

        updateUndoButton();
        updateGameDisplay();
    } catch (const std::exception& e) {
        qDebug() << "Error resetting game:" << e.what();
        if (gameWidget) {
            stackedWidget->removeWidget(gameWidget);
            delete gameWidget;
            gameWidget = nullptr;
        }
        setupGameScreen();
    }
}

void TicTacToeGUI::makeMove(int row, int col) {
    try {
        if (!gameBoard || gameEnded) return;

        std::vector<std::vector<char>> currentBoardState = gameBoard->getBoard();

        if (gameBoard->makeMove(row, col, currentPlayer)) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");

            Move move(row, col, currentPlayer, ss.str(), currentGameMoves.size() + 1);
            gameStateStack->pushMove(move, currentBoardState);

            currentGameMoves.push_back(move);

            if (gameButtons[row][col]) {
                QString symbol = (currentPlayer == 'X') ? "❌" : "⭕";
                gameButtons[row][col]->setText(symbol);
                gameButtons[row][col]->setEnabled(false);

                if (currentPlayer == 'X') {
                    gameButtons[row][col]->setStyleSheet("color: #e74c3c; font-weight: bold;");
                } else {
                    gameButtons[row][col]->setStyleSheet("color: #3498db; font-weight: bold;");
                }
            }

            updateUndoButton();
            checkGameEnd();

            if (!gameEnded) {
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                updateGameDisplay();

                if (currentGameMode == GameMode::PLAYER_VS_AI && currentPlayer != playerSymbol) {
                    setGameButtonsEnabled(false);
                    QTimer::singleShot(500, this, &TicTacToeGUI::makeAIMove);
                }
            }
        }
    } catch (const std::exception& e) {
        qDebug() << "Error making move:" << e.what();
    }
}

void TicTacToeGUI::checkGameEnd() {
    if (!gameBoard) return;

    GameResult result = gameBoard->checkWin();
    if (result != GameResult::ONGOING) {
        gameEnded = true;
        setGameButtonsEnabled(false);

        QString message;
        QString notificationType;

        switch (result) {
        case GameResult::PLAYER1_WIN:
            if (currentGameMode == GameMode::PLAYER_VS_AI && playerSymbol == 'X') {
                message = "🎉 تهانينا! لقد فزت!";
                notificationType = "win";
            } else {
                message = "🎉!X لقد فاز اللاعب";
                notificationType = "win";
            }
            break;
        case GameResult::PLAYER2_WIN:
            if (currentGameMode == GameMode::PLAYER_VS_AI && playerSymbol == 'O') {
                message = "🎉 تهانينا! لقد فزت!";
                notificationType = "win";
            } else if (currentGameMode == GameMode::PLAYER_VS_AI) {
                message = "😞 فاز الكمبيوتر!";
                notificationType = "lose";
            } else {
                message = "🎉 !O فاز اللاعب ";
                notificationType = "win";
            }
            break;
        case GameResult::TIE:
            message = "🤝 تعادل!";
            notificationType = "tie";
            break;
        default:
            break;
        }

        if (gameStatusLabel) {
            gameStatusLabel->setText(message);
        }

        // عرض النافذة المنبثقة
        showGameNotification(message, notificationType);

        updateUserStats(result);
        saveGameRecord();
    }
}

void TicTacToeGUI::updateGameDisplay() {
    if (!currentPlayerLabel) return;

    if (gameEnded) return;

    QString playerText;
    if (currentGameMode == GameMode::PLAYER_VS_PLAYER) {
        playerText = QString("دور اللاعب: %1").arg(currentPlayer == 'X' ? "❌" : "⭕");
    } else {
        if (currentPlayer == playerSymbol) {
            playerText = "دورك 👤";
        } else {
            playerText = "دور الكمبيوتر 🤖";
        }
    }
    currentPlayerLabel->setText(playerText);
}

void TicTacToeGUI::makeAIMove() {
    if (!aiPlayer || !gameBoard || gameEnded) return;

    auto move = aiPlayer->getBestMove(*gameBoard);
    if (move.first != -1 && move.second != -1) {
        makeMove(move.first, move.second);
    }

    setGameButtonsEnabled(true);
}

void TicTacToeGUI::setGameButtonsEnabled(bool enabled) {
    if (gameButtons.size() == 3) {
        for (int i = 0; i < 3; i++) {
            if (gameButtons[i].size() == 3) {
                for (int j = 0; j < 3; j++) {
                    if (gameButtons[i][j] && gameButtons[i][j]->text().isEmpty()) {
                        gameButtons[i][j]->setEnabled(enabled);
                    }
                }
            }
        }
    }
}

void TicTacToeGUI::undoLastMove() {
    try {
        if (!gameStateStack || !gameStateStack->canUndo() || gameEnded) {
            return;
        }

        Move lastMove = gameStateStack->popMove();
        std::vector<std::vector<char>> previousBoard = gameStateStack->popBoardState();

        gameBoard->setBoard(previousBoard);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                char cell = previousBoard[i][j];
                if (gameButtons[i][j]) {
                    if (cell == ' ') {
                        gameButtons[i][j]->setText("");
                        gameButtons[i][j]->setEnabled(true);
                        gameButtons[i][j]->setStyleSheet("");
                    } else {
                        QString symbol = (cell == 'X') ? "❌" : "⭕";
                        gameButtons[i][j]->setText(symbol);
                        gameButtons[i][j]->setEnabled(false);
                        if (cell == 'X') {
                            gameButtons[i][j]->setStyleSheet("color: #e74c3c; font-weight: bold;");
                        } else {
                            gameButtons[i][j]->setStyleSheet("color: #3498db; font-weight: bold;");
                        }
                    }
                }
            }
        }

        if (!currentGameMoves.empty()) {
            currentGameMoves.pop_back();
        }

        if (currentGameMode == GameMode::PLAYER_VS_AI && lastMove.player != playerSymbol) {
            if (gameStateStack->canUndo()) {
                Move playerMove = gameStateStack->popMove();
                std::vector<std::vector<char>> playerPreviousBoard = gameStateStack->popBoardState();

                gameBoard->setBoard(playerPreviousBoard);

                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        char cell = playerPreviousBoard[i][j];
                        if (gameButtons[i][j]) {
                            if (cell == ' ') {
                                gameButtons[i][j]->setText("");
                                gameButtons[i][j]->setEnabled(true);
                                gameButtons[i][j]->setStyleSheet("");
                            } else {
                                QString symbol = (cell == 'X') ? "❌" : "⭕";
                                gameButtons[i][j]->setText(symbol);
                                gameButtons[i][j]->setEnabled(false);
                                if (cell == 'X') {
                                    gameButtons[i][j]->setStyleSheet("color: #e74c3c; font-weight: bold;");
                                } else {
                                    gameButtons[i][j]->setStyleSheet("color: #3498db; font-weight: bold;");
                                }
                            }
                        }
                    }
                }

                if (!currentGameMoves.empty()) {
                    currentGameMoves.pop_back();
                }
            }
        }

        currentPlayer = playerSymbol;
        gameEnded = false;

        setGameButtonsEnabled(true);
        updateUndoButton();
        updateGameDisplay();

    } catch (const std::exception& e) {
        qDebug() << "Error in undo move:" << e.what();
    }
}

void TicTacToeGUI::updateUndoButton() {
    if (undoButton) {
        bool canUndo = gameStateStack && gameStateStack->canUndo() && !gameEnded;
        undoButton->setEnabled(canUndo);
    }
}

void TicTacToeGUI::updateUserStats(GameResult result) {
    try {
        if (!userManager || currentUser.empty()) return;

        User* user = userManager->getUser(currentUser);
        if (user) {
            user->gamesPlayed++;

            switch (result) {
            case GameResult::HUMAN_WIN:
            case GameResult::PLAYER1_WIN:
            case GameResult::PLAYER2_WIN:
                if ((result == GameResult::PLAYER1_WIN && playerSymbol == 'X') ||
                    (result == GameResult::PLAYER2_WIN && playerSymbol == 'O')) {
                    user->gamesWon++;
                } else {
                    user->gamesLost++;
                }
                break;
            case GameResult::AI_WIN:
                user->gamesLost++;
                break;
            case GameResult::TIE:
                user->gamesTied++;
                break;
            default:
                break;
            }

            userManager->updateUser(currentUser, *user);
        }
    } catch (const std::exception& e) {
        qDebug() << "Error updating user stats:" << e.what();
    }
}

void TicTacToeGUI::saveGameRecord() {
    if (!gameHistory || !gameBoard) return;

    GameRecord record;
    record.player1 = currentUser;
    record.player2 = (currentGameMode == GameMode::PLAYER_VS_AI) ? "Computer" : "Player2";
    record.mode = currentGameMode;
    record.result = gameBoard->checkWin();
    record.finalBoard = gameBoard->getBoard();
    record.moves = currentGameMoves;

    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    record.timestamp = ss.str();

    gameHistory->addGameRecord(record);
}

void TicTacToeGUI::updateHistoryDisplay() {
    if (!historyList || !gameHistory) return;

    historyList->clear();
    auto userGames = gameHistory->getUserGames(currentUser);

    for (const auto& record : userGames) {
        QString itemText = QString("%1 - %2")
        .arg(QString::fromStdString(record.timestamp))
            .arg(record.result == GameResult::PLAYER1_WIN ? "فوز" :
                     record.result == GameResult::PLAYER2_WIN ? "فوز" :
                     record.result == GameResult::TIE ? "تعادل" : "جاري");
        historyList->addItem(itemText);
    }
}

void TicTacToeGUI::initializeUserManager() {
    if (!userManager) {
        userManager = std::make_unique<UserHashTable>();
    }
}

void TicTacToeGUI::initializeAIPlayer() {
    if (!aiPlayer) {
        char aiSymbol = (playerSymbol == 'X') ? 'O' : 'X';
        aiPlayer = std::make_unique<AIPlayer>(aiSymbol, playerSymbol, currentDifficulty);
    }
}

void TicTacToeGUI::initializeGameHistory() {
    if (!gameHistory) {
        gameHistory = std::make_unique<GameHistory>();
    }
}

void TicTacToeGUI::initializeAutoPlayTimer() {
    if (!autoPlayTimer) {
        autoPlayTimer = new QTimer(this);
        connect(autoPlayTimer, &QTimer::timeout, this, &TicTacToeGUI::autoPlayReplay);
    }
}

void TicTacToeGUI::applyTheme() {
    QString styleSheet;

    qDebug() << "Applying theme:" << static_cast<int>(currentTheme);

    if (currentTheme == ThemeMode::DARK) {
        // الوضع الداكن
        styleSheet = R"(
            QMainWindow {
                background-color: #2c3e50;
                color: #ecf0f1;
            }
            QWidget {
                background-color: transparent;
                color: #ecf0f1;
                font-family: 'Segoe UI', 'Tahoma', Arial, sans-serif;
                font-size: 14px;
            }
            QStackedWidget {
                background-color: #2c3e50;
            }
            #titleLabel {
                font-size: 28px;
                font-weight: bold;
                color: #3498db;
                margin: 25px;
                background-color: transparent;
            }
            #primaryButton {
                background-color: #3498db;
                color: white;
                border: none;
                padding: 12px 25px;
                font-size: 14px;
                font-weight: 600;
                border-radius: 20px;
                min-width: 180px;
                min-height: 40px;
            }
            #buttonContainer {
                background-color: #34495e;
                border: 2px solid #3498db;
                border-radius: 15px;
            }
            #primaryButton:hover {
                background-color: #2980b9;
            }
            #primaryButton:disabled {
                background-color: #7f8c8d;
                color: #bdc3c7;
            }
            #secondaryButton {
                background-color: #95a5a6;
                color: white;
                border: none;
                padding: 10px 20px;
                font-size: 13px;
                font-weight: 600;
                border-radius: 18px;
                min-width: 160px;
                min-height: 36px;
            }
            #secondaryButton:hover {
                background-color: #7f8c8d;
            }
            #secondaryButton:disabled {
                background-color: #7f8c8d;
                color: #bdc3c7;
            }
            #settingsButtonSmall {
                background-color: #e67e22;
                color: white;
                border: none;
                border-radius: 20px;
                font-size: 14px;
                font-weight: bold;
                min-width: 40px;
                min-height: 40px;
            }
            #settingsButtonSmall:hover {
                background-color: #d35400;
            }
            #backButton {
                background-color: #e74c3c;
                color: white;
                border: none;
                padding: 8px 16px;
                font-size: 12px;
                font-weight: 600;
                border-radius: 15px;
                min-height: 32px;
            }
            #backButton:hover {
                background-color: #c0392b;
            }
            #gameCell {
                background-color: #34495e;
                border: 2px solid #3498db;
                font-size: 24px;
                font-weight: bold;
                border-radius: 12px;
                color: white;
                min-width: 80px;
                min-height: 80px;
            }
            #gameCell:hover {
                background-color: #2c3e50;
                border-color: #e67e22;
            }
            #gameCell:disabled {
                border-color: #7f8c8d;
            }
            #inputField {
                padding: 10px;
                border: 2px solid #3498db;
                border-radius: 8px;
                background-color: #34495e;
                color: #ecf0f1;
                font-size: 14px;
                min-height: 20px;
            }
            #inputField:focus {
                border-color: #e67e22;
                background-color: #2c3e50;
            }
            #linkButton {
                background-color: transparent;
                color: #3498db;
                border: none;
                text-decoration: underline;
                font-size: 12px;
                padding: 5px;
            }
            #linkButton:hover {
                color: #e67e22;
            }
            #statusLabel {
                font-size: 16px;
                color: #3498db;
                font-weight: 600;
                margin: 5px;
                background-color: transparent;
            }
            #statsLabel {
                font-size: 13px;
                color: #95a5a6;
                margin: 8px;
                padding: 5px;
                background-color: transparent;
            }
            #settingsItemLabel {
                font-size: 14px;
                color: #ecf0f1;
                font-weight: 600;
                margin: 5px;
                background-color: transparent;
            }
            #settingsComboBox {
                padding: 8px;
                border: 2px solid #3498db;
                border-radius: 6px;
                background-color: #34495e;
                color: #ecf0f1;
                font-size: 14px;
                min-height: 20px;
            }
            #historyList {
                background-color: #34495e;
                border: 2px solid #3498db;
                border-radius: 8px;
                color: #ecf0f1;
                padding: 5px;
            }
            #historyList::item {
                padding: 8px;
                border-bottom: 1px solid #7f8c8d;
                background-color: transparent;
            }
            #historyList::item:selected {
                background-color: #3498db;
            }
            #gameDetails {
                background-color: #34495e;
                border: 2px solid #3498db;
                border-radius: 8px;
                color: #ecf0f1;
                padding: 10px;
                font-size: 13px;
            }
            #infoLabel {
                font-size: 12px;
                color: #95a5a6;
                margin: 3px;
                background-color: transparent;
            }
            QRadioButton {
                color: #ecf0f1;
                font-size: 14px;
                padding: 5px;
                background-color: transparent;
            }
            QRadioButton::indicator {
                width: 18px;
                height: 18px;
            }
            QRadioButton::indicator:unchecked {
                border: 2px solid #3498db;
                border-radius: 9px;
                background-color: transparent;
            }
            QRadioButton::indicator:checked {
                border: 2px solid #3498db;
                border-radius: 9px;
                background-color: #3498db;
            }
            #notificationLabel {
                background-color: #3498db;
                color: white;
                border: 2px solid #ecf0f1;
                border-radius: 12px;
                padding: 15px;
                font-size: 16px;
                font-weight: bold;
            }
            #notificationLabel[notificationType="win"] {
                background-color: #27ae60;
            }
            #notificationLabel[notificationType="lose"] {
                background-color: #e74c3c;
            }
            #notificationLabel[notificationType="tie"] {
                background-color: #f39c12;
            }
        )";
    } else {
        // الوضع الفاتح - مع التأكد من الخلفية البيضاء
        styleSheet = R"(
            QMainWindow {
                background-color: #f8f9fa;
                color: #2c3e50;
            }
            QWidget {
                background-color: transparent;
                color: #2c3e50;
                font-family: 'Segoe UI', 'Tahoma', Arial, sans-serif;
                font-size: 14px;
            }
            QStackedWidget {
                background-color: #f8f9fa;
            }
            #titleLabel {
                font-size: 28px;
                font-weight: bold;
                color: #2980b9;
                margin: 25px;
                background-color: transparent;
            }
            #primaryButton {
                background-color: #3498db;
                color: white;
                border: none;
                padding: 12px 25px;
                font-size: 14px;
                font-weight: 600;
                border-radius: 20px;
                min-width: 180px;
                min-height: 40px;
            }
            #primaryButton:hover {
                background-color: #2980b9;
            }
            #buttonContainer {
                background-color: white;
                border: 2px solid #3498db;
                border-radius: 15px;
            }
                        #primaryButton:disabled {
                background-color: #bdc3c7;
                color: #7f8c8d;
            }
            #secondaryButton {
                background-color: #95a5a6;
                color: white;
                border: none;
                padding: 10px 20px;
                font-size: 13px;
                font-weight: 600;
                border-radius: 18px;
                min-width: 160px;
                min-height: 36px;
            }
            #secondaryButton:hover {
                background-color: #7f8c8d;
            }
            #secondaryButton:disabled {
                background-color: #bdc3c7;
                color: #7f8c8d;
            }
            #settingsButtonSmall {
                background-color: #e67e22;
                color: white;
                border: none;
                border-radius: 20px;
                font-size: 14px;
                font-weight: bold;
                min-width: 40px;
                min-height: 40px;
            }
            #settingsButtonSmall:hover {
                background-color: #d35400;
            }
            #backButton {
                background-color: #e74c3c;
                color: white;
                border: none;
                padding: 8px 16px;
                font-size: 12px;
                font-weight: 600;
                border-radius: 15px;
                min-height: 32px;
            }
            #backButton:hover {
                background-color: #c0392b;
            }
            #gameCell {
                background-color: white;
                border: 2px solid #3498db;
                font-size: 24px;
                font-weight: bold;
                border-radius: 12px;
                color: #2c3e50;
                min-width: 80px;
                min-height: 80px;
            }
            #gameCell:hover {
                background-color: #f8f9fa;
                border-color: #e67e22;
            }
            #gameCell:disabled {
                border-color: #bdc3c7;
                background-color: #ecf0f1;
            }
            #inputField {
                padding: 10px;
                border: 2px solid #3498db;
                border-radius: 8px;
                background-color: white;
                color: #2c3e50;
                font-size: 14px;
                min-height: 20px;
            }
            #inputField:focus {
                border-color: #e67e22;
            }
            #linkButton {
                background-color: transparent;
                color: #3498db;
                border: none;
                text-decoration: underline;
                font-size: 12px;
                padding: 5px;
            }
            #linkButton:hover {
                color: #e67e22;
            }
            #statusLabel {
                font-size: 16px;
                color: #2980b9;
                font-weight: 600;
                margin: 5px;
                background-color: transparent;
            }
            #statsLabel {
                font-size: 13px;
                color: #7f8c8d;
                margin: 8px;
                padding: 5px;
                background-color: transparent;
            }
            #settingsItemLabel {
                font-size: 14px;
                color: #2c3e50;
                font-weight: 600;
                margin: 5px;
                background-color: transparent;
            }
            #settingsComboBox {
                padding: 8px;
                border: 2px solid #3498db;
                border-radius: 6px;
                background-color: white;
                color: #2c3e50;
                font-size: 14px;
                min-height: 20px;
            }
            #historyList {
                background-color: white;
                border: 2px solid #3498db;
                border-radius: 8px;
                color: #2c3e50;
                padding: 5px;
            }
            #historyList::item {
                padding: 8px;
                border-bottom: 1px solid #bdc3c7;
                background-color: transparent;
            }
            #historyList::item:selected {
                background-color: #3498db;
                color: white;
            }
            #historyList::item:hover {
                background-color: #ecf0f1;
            }
            #gameDetails {
                background-color: white;
                border: 2px solid #3498db;
                border-radius: 8px;
                color: #2c3e50;
                padding: 10px;
                font-size: 13px;
            }
            #infoLabel {
                font-size: 12px;
                color: #7f8c8d;
                margin: 3px;
                background-color: transparent;
            }
            QRadioButton {
                color: #2c3e50;
                font-size: 14px;
                padding: 5px;
                background-color: transparent;
            }
            QRadioButton::indicator {
                width: 18px;
                height: 18px;
            }
            QRadioButton::indicator:unchecked {
                border: 2px solid #3498db;
                border-radius: 9px;
                background-color: white;
            }
            QRadioButton::indicator:checked {
                border: 2px solid #3498db;
                border-radius: 9px;
                background-color: #3498db;
            }
            #notificationLabel {
                background-color: #3498db;
                color: white;
                border: 2px solid #2c3e50;
                border-radius: 12px;
                padding: 15px;
                font-size: 16px;
                font-weight: bold;
            }
            #notificationLabel[notificationType="win"] {
                background-color: #27ae60;
            }
            #notificationLabel[notificationType="lose"] {
                background-color: #e74c3c;
            }
            #notificationLabel[notificationType="tie"] {
                background-color: #f39c12;
            }
        )";
    }

    setStyleSheet(styleSheet);

    // إجبار إعادة تطبيق التنسيق على جميع العناصر
    if (stackedWidget) {
        stackedWidget->style()->unpolish(stackedWidget);
        stackedWidget->style()->polish(stackedWidget);
    }

    // تحديث جميع الويدجتس الفرعية
    QList<QWidget*> widgets = findChildren<QWidget*>();
    for (QWidget* widget : widgets) {
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
        widget->update();
    }

    // تحديث النافذة الرئيسية
    update();

    qDebug() << "Theme applied successfully:" << static_cast<int>(currentTheme);
}

void TicTacToeGUI::debugTheme() {
    qDebug() << "Current theme:" << static_cast<int>(currentTheme);
    qDebug() << "ComboBox index:" << (themeComboBox ? themeComboBox->currentIndex() : -1);
    qDebug() << "Main window background:" << palette().color(QPalette::Window).name();
}

void TicTacToeGUI::saveThemeSettings() {
    std::ofstream file("settings.dat");
    if (file.is_open()) {
        file << static_cast<int>(currentTheme) << std::endl;
        file.close();
    }
}

void TicTacToeGUI::loadThemeSettings() {
    // تعيين الافتراضي كفاتح
    currentTheme = ThemeMode::DARK;

    std::ifstream file("settings.dat");
    if (file.is_open()) {
        int theme;
        if (file >> theme) {
            // التأكد من أن القيمة صحيحة
            if (theme == 0 || theme == 1) {
                currentTheme = static_cast<ThemeMode>(theme);
            }
        }
        file.close();
    }

    qDebug() << "Theme loaded:" << static_cast<int>(currentTheme);
}

// Slot implementations
void TicTacToeGUI::onLoginClicked() {
    try {
        if (!loginUsernameEdit || !loginPasswordEdit) return;

        QString username = loginUsernameEdit->text().trimmed();
        QString password = loginPasswordEdit->text();

        if (username.isEmpty() || password.isEmpty()) {
            showGameNotification("يرجى ملء جميع الحقول", "error");
            return;
        }

        initializeUserManager();

        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(password.toUtf8());
        QString hashedPassword = hash.result().toHex();

        if (userManager && userManager->authenticateUser(username.toStdString(), hashedPassword.toStdString())) {
            currentUser = username.toStdString();
            initializeGameHistory();
            switchToScreen(GameState::GAME_MODE_SELECTION);
        } else {
            showGameNotification("اسم المستخدم أو كلمة المرور غير صحيحة", "error");
        }
    } catch (const std::exception& e) {
        qDebug() << "Error in login:" << e.what();
        showGameNotification("فشل في تسجيل الدخول", "error");
    }
}

void TicTacToeGUI::onRegisterClicked() {
    try {
        if (!registerUsernameEdit || !registerPasswordEdit || !registerConfirmPasswordEdit) return;

        QString username = registerUsernameEdit->text().trimmed();
        QString password = registerPasswordEdit->text();
        QString confirmPassword = registerConfirmPasswordEdit->text();

        if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
            showGameNotification("يرجى ملء جميع الحقول", "error");
            return;
        }

        if (password != confirmPassword) {
            showGameNotification("كلمات المرور غير متطابقة", "error");
            return;
        }

        if (password.length() < 4) {
            showGameNotification("كلمة المرور يجب أن تكون 4 أحرف على الأقل", "error");
            return;
        }

        initializeUserManager();

        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(password.toUtf8());
        QString hashedPassword = hash.result().toHex();

        if (userManager && userManager->insertUser(username.toStdString(), hashedPassword.toStdString())) {
            showGameNotification("تم إنشاء الحساب بنجاح!", "success");
            switchToScreen(GameState::LOGIN);
        } else {
            showGameNotification("اسم المستخدم موجود مسبقاً", "error");
        }
    } catch (const std::exception& e) {
        qDebug() << "Error in registration:" << e.what();
        showGameNotification("فشل في إنشاء الحساب", "error");
    }
}

void TicTacToeGUI::onBackToMenuClicked() {
    switchToScreen(GameState::MENU);
}

void TicTacToeGUI::onPlayerVsPlayerClicked() {
    currentGameMode = GameMode::PLAYER_VS_PLAYER;
    switchToScreen(GameState::SYMBOL_SELECTION);
}

void TicTacToeGUI::onPlayerVsAIClicked() {
    currentGameMode = GameMode::PLAYER_VS_AI;
    switchToScreen(GameState::SYMBOL_SELECTION);
}

void TicTacToeGUI::onGameHistoryClicked() {
    switchToScreen(GameState::GAME_HISTORY);
}

void TicTacToeGUI::onCellClicked() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button || gameEnded) return;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (gameButtons[i][j] == button) {
                makeMove(i, j);
                return;
            }
        }
    }
}

void TicTacToeGUI::onNewGameClicked() {
    resetGame();
}

void TicTacToeGUI::onLogoutClicked() {
    currentUser.clear();
    switchToScreen(GameState::MENU);
}

void TicTacToeGUI::onShowRegisterClicked() {
    switchToScreen(GameState::REGISTER);
}

void TicTacToeGUI::onShowLoginClicked() {
    switchToScreen(GameState::LOGIN);
}

void TicTacToeGUI::onReplayGameClicked() {
    if (!historyList || !gameHistory) return;

    int index = historyList->currentRow();
    if (index >= 0) {
        auto userGames = gameHistory->getUserGames(currentUser);
        if (index < userGames.size()) {
            startReplay(userGames[index]);
        }
    }
}

void TicTacToeGUI::onReplayNextClicked() {
    if (currentReplayMoveIndex < currentReplayGame.moves.size()) {
        showReplayMove(currentReplayMoveIndex);
        currentReplayMoveIndex++;
        updateReplayDisplay();
    }
}

void TicTacToeGUI::onReplayPrevClicked() {
    if (currentReplayMoveIndex > 0) {
        currentReplayMoveIndex--;
        resetReplayBoard();
        for (int i = 0; i < currentReplayMoveIndex; i++) {
            showReplayMove(i);
        }
        updateReplayDisplay();
    }
}

void TicTacToeGUI::onReplayBackClicked() {
    if (autoPlayTimer && autoPlayTimer->isActive()) {
        autoPlayTimer->stop();
        isAutoPlaying = false;
    }
    switchToScreen(GameState::GAME_HISTORY);
}

void TicTacToeGUI::onReplayAutoPlayClicked() {
    if (!autoPlayTimer) {
        initializeAutoPlayTimer();
    }

    if (isAutoPlaying) {
        autoPlayTimer->stop();
        isAutoPlaying = false;
        if (replayAutoPlayButton) {
            replayAutoPlayButton->setText("⏯️ تشغيل تلقائي");
        }
    } else {
        autoPlayTimer->start(1000);
        isAutoPlaying = true;
        if (replayAutoPlayButton) {
            replayAutoPlayButton->setText("⏸️ إيقاف");
        }
    }
}

void TicTacToeGUI::onSettingsClicked() {
    if (themeComboBox) {
        themeComboBox->setCurrentIndex(static_cast<int>(currentTheme));
    }
    switchToScreen(GameState::SETTINGS);
}

void TicTacToeGUI::onThemeChanged() {
    if (themeComboBox) {
        int selectedIndex = themeComboBox->currentIndex();
        qDebug() << "Theme changed to index:" << selectedIndex;

        // تحديد الثيم بناءً على الفهرس
        if (selectedIndex == 0) {
            currentTheme = ThemeMode::LIGHT;
        } else if (selectedIndex == 1) {
            currentTheme = ThemeMode::DARK;
        }

        qDebug() << "Current theme set to:" << static_cast<int>(currentTheme);

        // تطبيق الثيم فوراً
        applyTheme();

        // حفظ الإعدادات
        saveThemeSettings();
    }
}

void TicTacToeGUI::onBackFromSettingsClicked() {
    switchToScreen(GameState::MENU);
}

void TicTacToeGUI::onSymbolSelectionClicked() {
    if (symbolXRadio && symbolXRadio->isChecked()) {
        playerSymbol = 'X';
        secondPlayerSymbol = 'O';
    } else {
        playerSymbol = 'O';
        secondPlayerSymbol = 'X';
    }

    if (currentGameMode == GameMode::PLAYER_VS_AI) {
        switchToScreen(GameState::DIFFICULTY_SELECTION);
    } else {
        resetGame();
        switchToScreen(GameState::PLAYING);
    }
}

void TicTacToeGUI::onBackFromSymbolSelectionClicked() {
    switchToScreen(GameState::GAME_MODE_SELECTION);
}

void TicTacToeGUI::onDifficultySelectionClicked() {
    if (easyRadio && easyRadio->isChecked()) {
        currentDifficulty = DifficultyLevel::EASY;
    } else if (mediumRadio && mediumRadio->isChecked()) {
        currentDifficulty = DifficultyLevel::MEDIUM;
    } else {
        currentDifficulty = DifficultyLevel::HARD;
    }

    initializeAIPlayer();
    resetGame();
    switchToScreen(GameState::PLAYING);
}

void TicTacToeGUI::onBackFromDifficultySelectionClicked() {
    switchToScreen(GameState::SYMBOL_SELECTION);
}

void TicTacToeGUI::onUndoMoveClicked() {
    undoLastMove();
}

void TicTacToeGUI::startReplay(const GameRecord& record) {
    currentReplayGame = record;
    currentReplayMoveIndex = 0;
    resetReplayBoard();
    switchToScreen(GameState::REPLAY_MODE);
    updateReplayDisplay();
}

void TicTacToeGUI::updateReplayDisplay() {
    if (replayMoveLabel) {
        QString moveText = QString("الحركة %1 من %2")
                               .arg(currentReplayMoveIndex)
                               .arg(currentReplayGame.moves.size());
        replayMoveLabel->setText(moveText);
    }

    if (replayInfoLabel) {
        QString info = QString("اللاعب الأول: %1 | اللاعب الثاني: %2")
                           .arg(QString::fromStdString(currentReplayGame.player1))
                           .arg(QString::fromStdString(currentReplayGame.player2));
        replayInfoLabel->setText(info);
    }

    if (replayPrevButton) {
        replayPrevButton->setEnabled(currentReplayMoveIndex > 0);
    }
    if (replayNextButton) {
        replayNextButton->setEnabled(currentReplayMoveIndex < currentReplayGame.moves.size());
    }
}

void TicTacToeGUI::resetReplayBoard() {
    replayBoard = std::vector<std::vector<char>>(3, std::vector<char>(3, ' '));

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (replayButtons[i][j]) {
                replayButtons[i][j]->setText("");
                replayButtons[i][j]->setStyleSheet("");
            }
        }
    }
}

void TicTacToeGUI::showReplayMove(int moveIndex) {
    if (moveIndex >= 0 && moveIndex < currentReplayGame.moves.size()) {
        const Move& move = currentReplayGame.moves[moveIndex];
        replayBoard[move.row][move.col] = move.player;

        if (replayButtons[move.row][move.col]) {
            QString symbol = (move.player == 'X') ? "❌" : "⭕";
            replayButtons[move.row][move.col]->setText(symbol);

            if (move.player == 'X') {
                replayButtons[move.row][move.col]->setStyleSheet("color: #e74c3c; font-weight: bold;");
            } else {
                replayButtons[move.row][move.col]->setStyleSheet("color: #3498db; font-weight: bold;");
            }
        }
    }
}

void TicTacToeGUI::autoPlayReplay() {
    if (currentReplayMoveIndex < currentReplayGame.moves.size()) {
        showReplayMove(currentReplayMoveIndex);
        currentReplayMoveIndex++;
        updateReplayDisplay();
    } else {
        if (autoPlayTimer) {
            autoPlayTimer->stop();
        }
        isAutoPlaying = false;
        if (replayAutoPlayButton) {
            replayAutoPlayButton->setText("⏯️ تشغيل تلقائي");
        }
    }
}

void TicTacToeGUI::applyReplayMove(int moveIndex) {
    showReplayMove(moveIndex);
}

void TicTacToeGUI::loadGameHistory() {
    // This function is called automatically when GameHistory is initialized
}
