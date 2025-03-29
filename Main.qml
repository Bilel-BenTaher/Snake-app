import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import SnakeGame 1.0  // Importation de notre composant C++

/**
 * Fenêtre principale du jeu Snake
 */
Window {
    id: mainWindow
    width: 420
    height: 500
    visible: true
    title: qsTr("Snake Game")  // Texte traduisible

    // Instance du jeu Snake (backend C++)
    Snake {
        id: snakeGame
    }

    /**
     * Panel des paramètres (masqué par défaut)
     */
    Rectangle {
        id: settingsPanel
        width: 300
        height: 200
        color: "#333333"  // Fond gris foncé
        border.color: "white"
        border.width: 2
        radius: 10        // Coins arrondis
        visible: false    // Masqué par défaut
        anchors.centerIn: parent
        z: 10             // Au-dessus des autres éléments

        // Contenu des paramètres
        Column {
            anchors.centerIn: parent
            spacing: 15   // Espacement vertical entre éléments

            // Titre des paramètres
            Text {
                text: "PARAMÈTRES"
                color: "white"
                font.pixelSize: 20
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Contrôle de vitesse
            Row {
                spacing: 10
                Text {
                    text: "Vitesse:"
                    color: "white"
                    font.pixelSize: 16
                    anchors.verticalCenter: parent.verticalCenter
                }
                Slider {
                    id: speedSlider
                    from: 300  // Lent
                    to: 50     // Rapide
                    value: snakeGame.gameSpeed
                    stepSize: 10
                    onMoved: snakeGame.setGameSpeed(value)
                }
            }

            // Affichage du meilleur score
            Text {
                text: "Meilleur score: " + snakeGame.highScore
                color: "white"
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Bouton de fermeture
            Button {
                text: "Fermer"
                onClicked: settingsPanel.visible = false
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    /**
     * Zone de jeu principale
     */
    Rectangle {
        anchors.fill: parent
        color: "black"  // Fond noir

        // Zone de jeu délimitée
        Rectangle {
            id: gameArea
            width: 400
            height: 400
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
            color: "#222222"  // Fond gris très foncé
            border.color: "white"
            border.width: 2

            // Nourriture (pomme)
            Rectangle {
                x: snakeGame.food.x
                y: snakeGame.food.y
                width: snakeGame.food.width
                height: snakeGame.food.height
                radius: 5  // Coins arrondis
                color: "red"
            }

            // Corps du serpent (utilise un Repeater pour chaque segment)
            Repeater {
                model: snakeGame.snakeBody  // Modèle lié au backend
                delegate: Rectangle {
                    x: modelData.x - 5  // Centrage
                    y: modelData.y - 5
                    width: 10
                    height: 10
                    radius: 3  // Légèrement arrondi
                    color: "lime"  // Vert vif
                }
            }

            // Overlay de pause
            Rectangle {
                visible: snakeGame.paused && snakeGame.gameState === "running"
                anchors.fill: parent
                color: "#80000000"  // Noir semi-transparent
                z: 1  // Au-dessus du jeu
                Text {
                    text: "PAUSE"
                    color: "white"
                    font.pixelSize: 48
                    font.bold: true
                    anchors.centerIn: parent
                }
            }
        }

        // Affichage du score
        Text {
            id: scoreText
            text: "Score: " + snakeGame.score + " | Meilleur: " + snakeGame.highScore
            color: "white"
            font.pixelSize: 20
            anchors.top: gameArea.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
        }

        /**
         * Barre de boutons de contrôle
         */
        Row {
            id: buttonRow
            anchors.top: scoreText.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20  // Espacement horizontal

            // Bouton Paramètres
            Button {
                id: settingsButton
                text: "Paramètres"
                onClicked: settingsPanel.visible = true
            }

            // Bouton Start/Reset
            Button {
                id: startButton
                text: {
                    // Texte dynamique selon l'état du jeu
                    if (snakeGame.gameState === "init") return "Nouvelle Partie"
                    if (snakeGame.gameState === "over") return "Rejouer"
                    return "Recommencer"
                }
                onClicked: {
                    snakeGame.resetGame()
                    snakeGame.startGame()
                }
            }

            // Bouton Pause
            Button {
                id: pauseButton
                text: snakeGame.paused ? "Reprendre" : "Pause"
                onClicked: snakeGame.togglePause()
                enabled: snakeGame.gameState === "running"  // Actif seulement en jeu
            }
        }

        // Message de fin de jeu
        Text {
            id: gameOverText
            text: snakeGame.gameState === "over" ? "Game Over! Score: " + snakeGame.score : ""
            color: "red"
            font.pixelSize: 24
            anchors.top: buttonRow.bottom
            anchors.topMargin: 15
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Gestion des touches clavier
        focus: true
        Keys.onPressed: {
            switch(event.key) {
            case Qt.Key_Left:
                if (!snakeGame.paused && snakeGame.gameState === "running")
                    snakeGame.changeDirection(Snake.MOVE_LEFT)
                break
            case Qt.Key_Right:
                if (!snakeGame.paused && snakeGame.gameState === "running")
                    snakeGame.changeDirection(Snake.MOVE_RIGHT)
                break
            case Qt.Key_Up:
                if (!snakeGame.paused && snakeGame.gameState === "running")
                    snakeGame.changeDirection(Snake.MOVE_UP)
                break
            case Qt.Key_Down:
                if (!snakeGame.paused && snakeGame.gameState === "running")
                    snakeGame.changeDirection(Snake.MOVE_DOWN)
                break
            case Qt.Key_P:  // Touche P pour pause
                if (snakeGame.gameState === "running")
                    snakeGame.togglePause()
                break
            case Qt.Key_Space:  // Espace pour démarrer/reprendre
                if (snakeGame.gameState === "running") {
                    snakeGame.resetGame()
                } else {
                    snakeGame.resetGame()
                    snakeGame.startGame()
                }
                break
            }
        }
    }
}
