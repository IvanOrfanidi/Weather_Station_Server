// Name of the executable file
def EXECUTABLE_FILE_NAME = "weather-station-server"
def CONFIGURATION_FILE_NAME = "configuration_of_weather_station_server"

def LOGIN = "pi"
def SERVER = "pijenkins.home.lan"
def PATH = "Weather_Station_Server/build"

// Program version
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
                sh 'cppcheck --language=c++ -Iinclude --enable=all --xml --xml-version=2 -i ext  source include 2> build/cppcheck.xml'
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

        stage('download configuration')
        {
            steps {
                script {
                    sh "rm -rf ${CONFIGURATION_FILE_NAME}"
                    sh "git clone http://git.home.lan/root/${CONFIGURATION_FILE_NAME}.git"
                }
            }
        }

        stage('archive') {
            steps {
                script {
                    VERSION = sh( returnStdout: true, script: "build/${EXECUTABLE_FILE_NAME} -v" ).trim()
                    sh "rm -rf build/config && mkdir build/config"
                    sh "cp -r ${CONFIGURATION_FILE_NAME}/*.cfg build/config/"
                    sh "zip build/${EXECUTABLE_FILE_NAME}-${VERSION}.zip build/${EXECUTABLE_FILE_NAME}.exe build/${EXECUTABLE_FILE_NAME} build/config/*.cfg"
                }
            }
        }

        stage('copy to server') {
            steps {
                script {
                    // Copying an executable file
                    sh "scp build/${EXECUTABLE_FILE_NAME} ${LOGIN}@${SERVER}:/home/${LOGIN}/${PATH}"
                    // Copying configuration files
                    sh "scp -r build/config ${LOGIN}@${SERVER}:/home/${LOGIN}/${PATH}/"
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
        buildDiscarder(logRotator(numToKeepStr: '1'))
    }
}
