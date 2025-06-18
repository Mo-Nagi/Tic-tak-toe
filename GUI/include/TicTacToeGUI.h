#ifndef TICTACTOEGUI_H
#define TICTACTOEGUI_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>
#include <QtCore/QTimer>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtCore/QCryptographicHash>

#include <vector>
#include <string>
#include <memory>

// Core classes (نظيف من غير ../../)
#include "GameBoard.h"
#include "AIPlayer.h"
#include "UserManager.h"
#include "GameHistory.h"
#include "GameStateStack.h"

enum class GameState {
    MENU,
    LOGIN,
    REGISTER,
    GAME_MODE_SELECTION,
    SYMBOL_SELECTION,
    DIFFICULTY_SELECTION,
    PLAYING,
    GAME_HISTORY,
    REPLAY_MODE,
    SETTINGS
};

enum class ThemeMode {
    LIGHT,
    DARK
};

class TicTacToeGUI : public QMainWindow {
    Q_OBJECT

public:
    TicTacToeGUI(QWidget *parent = nullptr);
    ~TicTacToeGUI();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onBackToMenuClicked();
    void onPlayerVsPlayerClicked();
    void onPlayerVsAIClicked();
    void onGameHistoryClicked();
    void onCellClicked();
    void onNewGameClicked();
    void onLogoutClicked();
    void onShowRegisterClicked();
    void onShowLoginClicked();
    void onReplayGameClicked();
    void onReplayNextClicked();
    void onReplayPrevClicked();
    void onReplayBackClicked();
    void onReplayAutoPlayClicked();
    void onSettingsClicked();
    void onThemeChanged();
    void onBackFromSettingsClicked();
    void onSymbolSelectionClicked();
    void onBackFromSymbolSelectionClicked();
    void onDifficultySelectionClicked();
    void onBackFromDifficultySelectionClicked();
    void onUndoMoveClicked();

private:
    void setupUI();
    void setupMenuScreen();
    void setupLoginScreen();
    void setupRegisterScreen();
    void setupGameModeScreen();
    void setupSymbolSelectionScreen();
    void setupDifficultySelectionScreen();
    void setupGameScreen();
    void setupHistoryScreen();
    void setupReplayScreen();
    void setupSettingsScreen();
    void switchToScreen(GameState state);
    void resetGame();
    void makeMove(int row, int col);
    void checkGameEnd();
    void updateGameDisplay();
    void updateUserStats(GameResult result);
    void saveGameRecord();
    void loadGameHistory();
    void updateHistoryDisplay();
    void makeAIMove();
    void setGameButtonsEnabled(bool enabled);
    void initializeUserManager();
    void initializeAIPlayer();
    void initializeGameHistory();
    void initializeAutoPlayTimer();
    void applyTheme();
    void saveThemeSettings();
    void loadThemeSettings();
    void startReplay(const GameRecord& record);
    void updateReplayDisplay();
    void resetReplayBoard();
    void applyReplayMove(int moveIndex);
    void showReplayMove(int moveIndex);
    void autoPlayReplay();
    void undoLastMove();
    void updateUndoButton();
    void updateGameModeDisplay();
    void debugTheme();

    void setupNotificationSystem();
    void showGameNotification(const QString& message, const QString& type);
    void hideNotification();

    std::unique_ptr<GameBoard> gameBoard;
    std::unique_ptr<AIPlayer> aiPlayer;
    std::unique_ptr<UserHashTable> userManager;
    std::unique_ptr<GameHistory> gameHistory;
    std::unique_ptr<GameStateStack> gameStateStack;

    QStackedWidget* stackedWidget;
    QWidget* menuWidget;
    QWidget* loginWidget;
    QWidget* registerWidget;
    QWidget* gameModeWidget;
    QWidget* symbolSelectionWidget;
    QWidget* difficultySelectionWidget;
    QWidget* gameWidget;
    QWidget* historyWidget;
    QWidget* replayWidget;
    QWidget* settingsWidget;

    QPushButton* loginButton;
    QPushButton* registerButton;
    QPushButton* settingsButton;
    QLabel* titleLabel;
    QComboBox* themeComboBox;
    QPushButton* settingsBackButton;
    QLabel* settingsLabel;
    QLineEdit* loginUsernameEdit;
    QLineEdit* loginPasswordEdit;
    QPushButton* loginSubmitButton;
    QPushButton* loginBackButton;
    QPushButton* showRegisterButton;
    QLineEdit* registerUsernameEdit;
    QLineEdit* registerPasswordEdit;
    QLineEdit* registerConfirmPasswordEdit;
    QPushButton* registerSubmitButton;
    QPushButton* registerBackButton;
    QPushButton* showLoginButton;
    QPushButton* playerVsPlayerButton;
    QPushButton* playerVsAIButton;
    QPushButton* gameHistoryButton;
    QPushButton* logoutButton;
    QLabel* welcomeLabel;
    QLabel* statsLabel;
    QRadioButton* symbolXRadio;
    QRadioButton* symbolORadio;
    QButtonGroup* symbolButtonGroup;
    QPushButton* symbolConfirmButton;
    QPushButton* symbolBackButton;
    QLabel* symbolSelectionLabel;
    QRadioButton* easyRadio;
    QRadioButton* mediumRadio;
    QRadioButton* hardRadio;
    QButtonGroup* difficultyButtonGroup;
    QPushButton* difficultyConfirmButton;
    QPushButton* difficultyBackButton;
    QLabel* difficultySelectionLabel;
    std::vector<std::vector<QPushButton*>> gameButtons;
    QLabel* gameStatusLabel;
    QLabel* currentPlayerLabel;
    QPushButton* newGameButton;
    QPushButton* backToModeButton;
    QPushButton* undoButton;
    QListWidget* historyList;
    QTextEdit* gameDetailsText;
    QPushButton* historyBackButton;
    QPushButton* replayGameButton;
    std::vector<std::vector<QPushButton*>> replayButtons;
    QLabel* replayStatusLabel;
    QLabel* replayMoveLabel;
    QPushButton* replayNextButton;
    QPushButton* replayPrevButton;
    QPushButton* replayBackButton;
    QPushButton* replayAutoPlayButton;
    QLabel* replayInfoLabel;
    QTimer* notificationTimer;
    QLabel* notificationLabel;

    GameState currentState;
    GameMode currentGameMode;
    std::string currentUser;
    char currentPlayer;
    char playerSymbol;
    char secondPlayerSymbol;
    bool gameEnded;
    std::vector<Move> currentGameMoves;
    ThemeMode currentTheme;
    DifficultyLevel currentDifficulty;

    GameRecord currentReplayGame;
    int currentReplayMoveIndex;
    bool isAutoPlaying;
    QTimer* autoPlayTimer;
    std::vector<std::vector<char>> replayBoard;
};

#endif // TICTACTOEGUI_H
