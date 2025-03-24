import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import SnakeGame 1.0

Window {
    width: 420
    height: 500
    visible: true
    title: qsTr("Snake Game")

    Snake {
        id: snakeGame
    }

    Rectangle {
        id: settingsPanel
        width: 300
        height: 200
        color: "#333333"
        border.color: "white"
        border.width: 2
        radius: 10
        visible: false
        anchors.centerIn: parent
        z: 10

        Column {
            anchors.centerIn: parent
            spacing: 15

            Text {
                text: "PARAMÈTRES"
                color: "white"
                font.pixelSize: 20
                anchors.horizontalCenter: parent.horizontalCenter
            }

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
                    from: 300
                    to: 50
                    value: snakeGame.gameSpeed
                    stepSize: 10
                    onMoved: snakeGame.setGameSpeed(value)
                }
            }

            Text {
                text: "Meilleur score: " + snakeGame.highScore
                color: "white"
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Button {
                text: "Fermer"
                onClicked: settingsPanel.visible = false
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "black"

        Rectangle {
            id: gameArea
            width: 400
            height: 400
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
            color: "#222222"
            border.color: "white"
            border.width: 2

            Rectangle {
                x: snakeGame.food.x
                y: snakeGame.food.y
                width: snakeGame.food.width
                height: snakeGame.food.height
                radius: 5
                color: "red"
            }

            Repeater {
                model: snakeGame.snakeBody
                delegate: Rectangle {
                    x: modelData.x - 5
                    y: modelData.y - 5
                    width: 10
                    height: 10
                    radius: 3
                    color: "lime"
                }
            }

            Rectangle {
                visible: snakeGame.paused && snakeGame.gameState === "running"
                anchors.fill: parent
                color: "#80000000"
                z: 1
                Text {
                    text: "PAUSE"
                    color: "white"
                    font.pixelSize: 48
                    font.bold: true
                    anchors.centerIn: parent
                }
            }
        }

        Text {
            id: scoreText
            text: "Score: " + snakeGame.score + " | Meilleur: " + snakeGame.highScore
            color: "white"
            font.pixelSize: 20
            anchors.top: gameArea.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Conteneur horizontal pour les boutons
        Row {
            id: buttonRow
            anchors.top: scoreText.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20

            // Bouton Paramètres (gauche)
            Button {
                id: settingsButton
                text: "Paramètres"
                onClicked: settingsPanel.visible = true
            }

            // Bouton Start/Centre (centre)
            Button {
                id: startButton
                text: {
                    if (snakeGame.gameState === "init") return "Nouvelle Partie"
                    if (snakeGame.gameState === "over") return "Rejouer"
                    return "Recommencer"
                }
                onClicked: {
                    snakeGame.resetGame()
                    snakeGame.startGame()
                }
            }

            // Bouton Pause (droite)
            Button {
                id: pauseButton
                text: snakeGame.paused ? "Reprendre" : "Pause"
                onClicked: snakeGame.togglePause()
                enabled: snakeGame.gameState === "running"
            }
        }

        Text {
            id: gameOverText
            text: snakeGame.gameState === "over" ? "Game Over! Score: " + snakeGame.score : ""
            color: "red"
            font.pixelSize: 24
            anchors.top: buttonRow.bottom
            anchors.topMargin: 15
            anchors.horizontalCenter: parent.horizontalCenter
        }

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
            case Qt.Key_P:
                if (snakeGame.gameState === "running")
                    snakeGame.togglePause()
                break
            case Qt.Key_Space:
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
