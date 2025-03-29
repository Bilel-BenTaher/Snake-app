#include "snake.h"

/**
 * @brief Constructeur de la classe Snake
 * @param parent Parent QObject (optionnel)
 *
 * Initialise le jeu avec :
 * - État initial (GAME_INIT)
 * - Aucune direction (MOVE_NONE)
 * - Score à 0
 * - Générateur aléatoire sécurisé
 * - Chargement du meilleur score depuis QSettings
 */
Snake::Snake(QObject *parent) : QObject(parent),
    m_state(GAME_INIT),
    m_direction(MOVE_NONE),
    m_currentScore(0),
    m_randomGenerator(QRandomGenerator::securelySeeded()),
    m_settings("MyCompany", "SnakeGame")
{
    // Connexion du timer de jeu à la méthode updateGame
    connect(&m_gameTimer, &QTimer::timeout, this, &Snake::updateGame);

    // Chargement du meilleur score depuis les paramètres
    m_highScore = m_settings.value("highScore", 0).toInt();
}

/**
 * @brief Destructeur de la classe Snake
 */
Snake::~Snake()
{
    // Arrêt du timer si actif
    if (m_gameTimer.isActive()) {
        m_gameTimer.stop();
    }
}

// =============================================
// METHODES D'ACCES AUX PROPRIETES
// =============================================

/**
 * @brief Retourne le score actuel
 * @return Le score courant
 */
int Snake::score() const { return m_currentScore; }

/**
 * @brief Vérifie si le jeu est en pause
 * @return true si le jeu est en pause, false sinon
 */
bool Snake::isPaused() const { return m_paused; }

/**
 * @brief Retourne la vitesse actuelle du jeu
 * @return La vitesse en millisecondes entre les mouvements
 */
int Snake::gameSpeed() const { return m_gameSpeed; }

/**
 * @brief Retourne le meilleur score enregistré
 * @return Le meilleur score
 */
int Snake::highScore() const { return m_highScore; }

/**
 * @brief Convertit l'état du jeu en QString
 * @return Chaîne représentant l'état courant
 */
QString Snake::gameState() const {
    switch(m_state) {
    case GAME_INIT: return "init";
    case GAME_RUN: return "running";
    case GAME_OVER: return "over";
    case GAME_EXIT: return "exit";
    default: return "init";
    }
}

/**
 * @brief Retourne les segments du serpent
 * @return Vecteur des positions des segments
 */
QVector<QPoint> Snake::snakeBody() const { return m_snakeBody; }

/**
 * @brief Retourne la position de la nourriture
 * @return Rectangle représentant la nourriture
 */
QRect Snake::food() const { return m_food; }

// =============================================
// METHODES DE MODIFICATION
// =============================================

/**
 * @brief Modifie la vitesse du jeu
 * @param speed Nouvelle vitesse (50-300ms)
 */
void Snake::setGameSpeed(int speed) {
    if (speed != m_gameSpeed && speed >= 50 && speed <= 300) {
        m_gameSpeed = speed;
        if (m_gameTimer.isActive()) {
            m_gameTimer.start(m_gameSpeed);
        }
        emit gameSpeedChanged();
    }
}

/**
 * @brief Sauvegarde le meilleur score si nécessaire
 */
void Snake::saveHighScore() {
    if (m_currentScore > m_highScore) {
        m_highScore = m_currentScore;
        m_settings.setValue("highScore", m_highScore);
        emit highScoreChanged();
    }
}

// =============================================
// METHODES DE GESTION DU JEU
// =============================================

/**
 * @brief Démarre une nouvelle partie
 */
void Snake::startGame() {
    m_currentScore = 0;
    emit scoreChanged();
    initGame();
    initFood();
    m_state = GAME_RUN;
    emit gameStateChanged();
    m_gameTimer.start(m_gameSpeed);
}

/**
 * @brief Réinitialise le jeu à l'état initial
 */
void Snake::resetGame() {
    m_gameTimer.stop();
    m_snakeBody.clear();
    m_state = GAME_INIT;
    m_direction = MOVE_NONE;
    emit gameStateChanged();
    emit snakeBodyChanged();
}

/**
 * @brief Met en pause/reprend le jeu
 */
void Snake::togglePause() {
    m_paused = !m_paused;
    if (m_paused) {
        m_gameTimer.stop();
    } else {
        m_gameTimer.start(m_gameSpeed);
    }
    emit pausedChanged();
}

/**
 * @brief Change la direction du serpent
 * @param direction Nouvelle direction
 */
void Snake::changeDirection(Direction direction) {
    if (m_state != GAME_RUN) return;

    // Empêche les demi-tours brusques
    if ((m_direction == MOVE_LEFT && direction == MOVE_RIGHT) ||
        (m_direction == MOVE_RIGHT && direction == MOVE_LEFT) ||
        (m_direction == MOVE_UP && direction == MOVE_DOWN) ||
        (m_direction == MOVE_DOWN && direction == MOVE_UP)) return;

    m_direction = direction;
}

// =============================================
// METHODES INTERNES
// =============================================

/**
 * @brief Initialise le serpent en position de départ
 */
void Snake::initGame() {
    m_snakeBody.clear();
    // Crée un serpent de taille initiale horizontale
    for (int i = 0; i < m_initialSize; i++) {
        m_snakeBody.append(QPoint(m_step * (m_initialSize - i), m_step));
    }
    emit snakeBodyChanged();
}

/**
 * @brief Positionne la nourriture aléatoirement
 */
void Snake::initFood() {
    bool success = false;
    while (!success) {
        // Génère des coordonnées aléatoires
        int x = getRandomNumber(39, 1) * m_step;
        int y = getRandomNumber(39, 1) * m_step;
        m_food = QRect(x - 5, y - 5, 10, 10);

        // Vérifie que la nourriture n'est pas sur le serpent
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

/**
 * @brief Vérifie si le serpent a mangé la nourriture
 */
void Snake::evalFood() {
    if (m_food.contains(m_snakeBody.last())) {
        m_currentScore++;
        emit scoreChanged();

        // Ajoute un nouveau segment dans la direction actuelle
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
        initFood(); // Nouvelle nourriture
    }
}

/**
 * @brief Évalue l'état du jeu après un mouvement
 * @param pointNext Position suivante de la tête
 */
void Snake::evalGame(const QPoint &pointNext) {
    // Vérifie les collisions avec les bords ou le corps
    if (pointNext.x() < 0 || pointNext.x() >= 400 ||
        pointNext.y() < 0 || pointNext.y() >= 400 ||
        m_snakeBody.contains(pointNext)) {
        m_state = GAME_OVER;
        saveHighScore();
        emit gameStateChanged();
        m_gameTimer.stop();
    }
}

/**
 * @brief Met à jour l'état du jeu à chaque tick
 */
void Snake::updateGame() {
    if (m_state != GAME_RUN || m_paused) return;

    // Vérifie si la nourriture a été mangée
    bool ateFood = m_food.contains(m_snakeBody.last());
    if (ateFood) evalFood();

    // Calcule la nouvelle position de la tête
    QPoint head = m_snakeBody.last();
    QPoint newHead = head;
    switch(m_direction) {
    case MOVE_LEFT: newHead.rx() -= m_step; break;
    case MOVE_RIGHT: newHead.rx() += m_step; break;
    case MOVE_UP: newHead.ry() -= m_step; break;
    case MOVE_DOWN: newHead.ry() += m_step; break;
    default: return;
    }

    // Évalue le nouvel état
    evalGame(newHead);
    if (m_state == GAME_OVER) return;

    // Met à jour le serpent
    if (!ateFood) m_snakeBody.removeFirst(); // Pas de croissance si pas mangé
    m_snakeBody.append(newHead);
    emit snakeBodyChanged();
}

/**
 * @brief Génère un nombre aléatoire
 * @param limit Limite supérieure (exclusive)
 * @param offset Décalage (par défaut 0)
 * @return Nombre aléatoire entre offset et (offset + limit - 1)
 */
int Snake::getRandomNumber(int limit, int offset) const {
    return QRandomGenerator::global()->bounded(limit) + offset;
}
