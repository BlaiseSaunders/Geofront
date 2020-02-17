pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                echo 'Building..'
                sh("make")
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}
