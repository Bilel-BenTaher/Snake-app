#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>
#include <QVector>
#include <QPoint>
#include <QRect>
#include <QTimer>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QSettings>

class Snake : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(QString gameState READ gameState NOTIFY gameStateChanged)
    Q_PROPERTY(QVector<QPoint> snakeBody READ snakeBody NOTIFY snakeBodyChanged)
    Q_PROPERTY(QRect food READ food NOTIFY foodChanged)
    Q_PROPERTY(bool paused READ isPaused NOTIFY pausedChanged)
    Q_PROPERTY(int gameSpeed READ gameSpeed WRITE setGameSpeed NOTIFY gameSpeedChanged)
    Q_PROPERTY(int highScore READ highScore NOTIFY highScoreChanged)

public:
    explicit Snake(QObject *parent = nullptr);
    ~Snake();

    enum Direction {
        MOVE_NONE = 0,
        MOVE_LEFT = 1,
        MOVE_RIGHT = 2,
        MOVE_UP = 3,
        MOVE_DOWN = 4
    };
    Q_ENUM(Direction)

    enum GameState {
        GAME_INIT = 0,
        GAME_RUN = 1,
        GAME_OVER = 2,
        GAME_EXIT = 3
    };
    Q_ENUM(GameState)

    int score() const;
    QString gameState() const;
    QVector<QPoint> snakeBody() const;
    QRect food() const;
    bool isPaused() const;
    int gameSpeed() const;
    int highScore() const;

    Q_INVOKABLE void startGame();
    Q_INVOKABLE void changeDirection(Direction direction);
    Q_INVOKABLE void resetGame();
    Q_INVOKABLE void togglePause();
    Q_INVOKABLE void setGameSpeed(int speed);
    Q_INVOKABLE void saveHighScore();

signals:
    void scoreChanged();
    void gameStateChanged();
    void snakeBodyChanged();
    void foodChanged();
    void pausedChanged();
    void gameSpeedChanged();
    void highScoreChanged();

private:
    const static int m_step = 10;
    const static int m_initialSize = 3;
    bool m_paused = false;
    int m_gameSpeed = 150; // Valeur par défaut moyenne
    int m_highScore = 0;

    void initGame();
    void evalGame(const QPoint &pointNext);
    void initFood();
    void evalFood();
    int getRandomNumber(int limit, int offset = 0) const;

    QVector<QPoint> m_snakeBody;
    QRect m_food;
    GameState m_state;
    Direction m_direction;
    int m_currentScore;
    QTimer m_gameTimer;
    QElapsedTimer m_elapsedTimer;
    QRandomGenerator m_randomGenerator;
    QSettings m_settings;

private slots:
    void updateGame();
};

#endif // SNAKE_H
