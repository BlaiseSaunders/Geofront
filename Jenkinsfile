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

		sh("make -f Maketest")
		sh("./geo_test")
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}
