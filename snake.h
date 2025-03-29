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

/**
 * @class Snake
 * @brief Classe principale du jeu Snake implémentée en QObject pour Qt
 *
 * Cette classe gère toute la logique du jeu Snake, y compris :
 * - Le déplacement du serpent
 * - La génération de nourriture
 * - La détection des collisions
 * - La gestion des scores
 * - La persistance des paramètres
 */
class Snake : public QObject
{
    Q_OBJECT
    // Propriétés QML exposées
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

    /**
     * @enum Direction
     * @brief Directions possibles pour le serpent
     */
    enum Direction {
        MOVE_NONE = 0,  ///< Aucun mouvement
        MOVE_LEFT = 1,  ///< Vers la gauche
        MOVE_RIGHT = 2, ///< Vers la droite
        MOVE_UP = 3,    ///< Vers le haut
        MOVE_DOWN = 4   ///< Vers le bas
    };
    Q_ENUM(Direction)

    /**
     * @enum GameState
     * @brief États possibles du jeu
     */
    enum GameState {
        GAME_INIT = 0,  ///< État initial
        GAME_RUN = 1,   ///< Jeu en cours
        GAME_OVER = 2,  ///< Fin de partie
        GAME_EXIT = 3   ///< Jeu quitté
    };
    Q_ENUM(GameState)

    // Accesseurs pour les propriétés QML
    int score() const;               ///< Retourne le score actuel
    QString gameState() const;       ///< Retourne l'état du jeu sous forme de QString
    QVector<QPoint> snakeBody() const; ///< Retourne les segments du serpent
    QRect food() const;              ///< Retourne la position de la nourriture
    bool isPaused() const;           ///< Indique si le jeu est en pause
    int gameSpeed() const;           ///< Retourne la vitesse actuelle du jeu
    int highScore() const;           ///< Retourne le meilleur score enregistré

    // Méthodes invocables depuis QML
    Q_INVOKABLE void startGame();    ///< Démarre une nouvelle partie
    Q_INVOKABLE void changeDirection(Direction direction); ///< Change la direction du serpent
    Q_INVOKABLE void resetGame();    ///< Réinitialise le jeu
    Q_INVOKABLE void togglePause();  ///< Met en pause/reprend le jeu
    Q_INVOKABLE void setGameSpeed(int speed); ///< Modifie la vitesse du jeu
    Q_INVOKABLE void saveHighScore(); ///< Sauvegarde le meilleur score

signals:
    // Signaux émis lorsque les propriétés changent
    void scoreChanged();             ///< Émis lorsque le score change
    void gameStateChanged();         ///< Émis lorsque l'état du jeu change
    void snakeBodyChanged();         ///< Émis lorsque le serpent se déplace
    void foodChanged();              ///< Émis lorsque la nourriture est régénérée
    void pausedChanged();            ///< Émis lorsque l'état pause change
    void gameSpeedChanged();         ///< Émis lorsque la vitesse du jeu change
    void highScoreChanged();         ///< Émis lorsque le meilleur score est battu

private:
    // Constantes
    const static int m_step = 10;        ///< Taille d'un pas de déplacement
    const static int m_initialSize = 3;  ///< Taille initiale du serpent

    // Variables d'état
    bool m_paused = false;           ///< État de pause
    int m_gameSpeed = 150;           ///< Vitesse du jeu (ms entre les mouvements)
    int m_highScore = 0;             ///< Meilleur score enregistré

    // Méthodes internes
    void initGame();                 ///< Initialise les variables du jeu
    void evalGame(const QPoint &pointNext); ///< Évalue l'état du jeu après un mouvement
    void initFood();                 ///< Positionne la nourriture aléatoirement
    void evalFood();                 ///< Vérifie si la nourriture a été mangée
    int getRandomNumber(int limit, int offset = 0) const; ///< Génère un nombre aléatoire

    // Données du jeu
    QVector<QPoint> m_snakeBody;     ///< Segments du serpent (queue à tête)
    QRect m_food;                    ///< Position de la nourriture
    GameState m_state;               ///< État courant du jeu
    Direction m_direction;           ///< Direction courante du serpent
    int m_currentScore;              ///< Score actuel

    // Utilitaires Qt
    QTimer m_gameTimer;              ///< Timer pour les mouvements du serpent
    QElapsedTimer m_elapsedTimer;    ///< Mesure le temps écoulé
    QRandomGenerator m_randomGenerator; ///< Générateur de nombres aléatoires
    QSettings m_settings;            ///< Pour sauvegarder les paramètres

private slots:
    /**
     * @brief Met à jour l'état du jeu à chaque tick du timer
     *
     * Gère le déplacement du serpent et vérifie les collisions
     */
    void updateGame();
};

#endif // SNAKE_H
