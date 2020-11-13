// Имя выходного файла
def EXECUTABLE_FILE_NAME = "weather-station-server"

// Версия программы
def VERSION = ""

pipeline {
    agent any
    stages {
        stage('submodule update') {
            steps {
                sh 'git submodule init'
                sh 'git submodule update'
            }
        }

        stage('install libraries') {
            steps {
                sh 'bash install_lib.sh'
            }
        }

        stage('build') {
            steps {
                sh 'bash compile.sh'
            }
        }

        stage('cppcheck') {
            steps {
                sh 'cppcheck --enable=all --xml --xml-version=2 -i ext  source include 2> build/cppcheck.xml'
                sh 'cppcheck-htmlreport --source-encoding="iso8859-1" --title="project" --source-dir=. --report-dir=build --file=build/cppcheck.xml'
                publishHTML(
                    target: [
                      allowMissing: true,
                      alwaysLinkToLastBuild: false,
                      keepAll: false,
                      reportDir: 'build',
                      reportFiles: 'index.html',
                      reportName: 'CppCheck Report',
                      reportTitles: ''
                    ]
                )
            }
        }

        stage('archive') {
            steps {
                script {
                    VERSION = sh( returnStdout: true, script: "build/${EXECUTABLE_FILE_NAME} -v" ).trim()
                    sh 'git clone http://192.168.109.15/root/configuration_of_weather_station_server.git'
                    sh 'mkdir build/config'
                    sh 'cp -r configuration_of_weather_station_server/*.cfg build/config/'
                    sh "zip build/${EXECUTABLE_FILE_NAME}-${VERSION}.zip build/${EXECUTABLE_FILE_NAME}.exe build/${EXECUTABLE_FILE_NAME} build/config/*.cfg"
                }
            }
        }

        stage('artifacts') {
            steps {
                archiveArtifacts artifacts: "build/${EXECUTABLE_FILE_NAME}-${VERSION}.zip", onlyIfSuccessful: true
            }
        }

        stage('clean') {
            steps {
                cleanWs()
            }
        }
    }
    options {
        // Хранить только одну удачную сбороку
        buildDiscarder(logRotator(numToKeepStr: '1'))
    }
}
