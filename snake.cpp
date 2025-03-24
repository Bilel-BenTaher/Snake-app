#include "snake.h"

Snake::Snake(QObject *parent) : QObject(parent),
    m_state(GAME_INIT),
    m_direction(MOVE_NONE),
    m_currentScore(0),
    m_randomGenerator(QRandomGenerator::securelySeeded()),
    m_settings("MyCompany", "SnakeGame")
{
    connect(&m_gameTimer, &QTimer::timeout, this, &Snake::updateGame);
    m_highScore = m_settings.value("highScore", 0).toInt();
}

Snake::~Snake()
{
}

int Snake::score() const { return m_currentScore; }
bool Snake::isPaused() const { return m_paused; }
int Snake::gameSpeed() const { return m_gameSpeed; }
int Snake::highScore() const { return m_highScore; }

QString Snake::gameState() const {
    switch(m_state) {
    case GAME_INIT: return "init";
    case GAME_RUN: return "running";
    case GAME_OVER: return "over";
    case GAME_EXIT: return "exit";
    default: return "init";
    }
}

QVector<QPoint> Snake::snakeBody() const { return m_snakeBody; }
QRect Snake::food() const { return m_food; }

void Snake::setGameSpeed(int speed) {
    if (speed != m_gameSpeed && speed >= 50 && speed <= 300) {
        m_gameSpeed = speed;
        if (m_gameTimer.isActive()) {
            m_gameTimer.start(m_gameSpeed);
        }
        emit gameSpeedChanged();
    }
}

void Snake::saveHighScore() {
    if (m_currentScore > m_highScore) {
        m_highScore = m_currentScore;
        m_settings.setValue("highScore", m_highScore);
        emit highScoreChanged();
    }
}

void Snake::startGame() {
    m_currentScore = 0;
    emit scoreChanged();
    initGame();
    initFood();
    m_state = GAME_RUN;
    emit gameStateChanged();
    m_gameTimer.start(m_gameSpeed);
}

void Snake::resetGame() {
    m_gameTimer.stop();
    m_snakeBody.clear();
    m_state = GAME_INIT;
    m_direction = MOVE_NONE;
    emit gameStateChanged();
    emit snakeBodyChanged();
}

void Snake::togglePause() {
    m_paused = !m_paused;
    if (m_paused) {
        m_gameTimer.stop();
    } else {
        m_gameTimer.start(m_gameSpeed);
    }
    emit pausedChanged();
}

void Snake::changeDirection(Direction direction) {
    if (m_state != GAME_RUN) return;
    if ((m_direction == MOVE_LEFT && direction == MOVE_RIGHT) ||
        (m_direction == MOVE_RIGHT && direction == MOVE_LEFT) ||
        (m_direction == MOVE_UP && direction == MOVE_DOWN) ||
        (m_direction == MOVE_DOWN && direction == MOVE_UP)) return;
    m_direction = direction;
}

void Snake::initGame() {
    m_snakeBody.clear();
    for (int i = 0; i < m_initialSize; i++) {
        m_snakeBody.append(QPoint(m_step * (m_initialSize - i), m_step));
    }
    emit snakeBodyChanged();
}

void Snake::initFood() {
    bool success = false;
    while (!success) {
        int x = getRandomNumber(39, 1) * m_step;
        int y = getRandomNumber(39, 1) * m_step;
        m_food = QRect(x - 5, y - 5, 10, 10);
        success = true;
        for (const QPoint &point : m_snakeBody) {
            if (m_food.contains(point)) {
                success = false;
                break;
            }
        }
    }
    emit foodChanged();
}

void Snake::evalFood() {
    if (m_food.contains(m_snakeBody.last())) {
        m_currentScore++;
        emit scoreChanged();
        QPoint newSegment = m_snakeBody.last();
        switch(m_direction) {
        case MOVE_LEFT: newSegment.rx() -= m_step; break;
        case MOVE_RIGHT: newSegment.rx() += m_step; break;
        case MOVE_UP: newSegment.ry() -= m_step; break;
        case MOVE_DOWN: newSegment.ry() += m_step; break;
        default: break;
        }
        m_snakeBody.append(newSegment);
        emit snakeBodyChanged();
        initFood();
    }
}

void Snake::evalGame(const QPoint &pointNext) {
    if (pointNext.x() < 0 || pointNext.x() >= 400 ||
        pointNext.y() < 0 || pointNext.y() >= 400 ||
        m_snakeBody.contains(pointNext)) {
        m_state = GAME_OVER;
        saveHighScore();
        emit gameStateChanged();
        m_gameTimer.stop();
    }
}

void Snake::updateGame() {
    if (m_state != GAME_RUN || m_paused) return;

    bool ateFood = m_food.contains(m_snakeBody.last());
    if (ateFood) evalFood();

    QPoint head = m_snakeBody.last();
    QPoint newHead = head;
    switch(m_direction) {
    case MOVE_LEFT: newHead.rx() -= m_step; break;
    case MOVE_RIGHT: newHead.rx() += m_step; break;
    case MOVE_UP: newHead.ry() -= m_step; break;
    case MOVE_DOWN: newHead.ry() += m_step; break;
    default: return;
    }

    evalGame(newHead);
    if (m_state == GAME_OVER) return;

    if (!ateFood) m_snakeBody.removeFirst();
    m_snakeBody.append(newHead);
    emit snakeBodyChanged();
}

int Snake::getRandomNumber(int limit, int offset) const {
    return QRandomGenerator::global()->bounded(limit) + offset;
}
