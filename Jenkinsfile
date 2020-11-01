pipeline {
  agent any
  stages {
    stage('Create Container Image') {
      steps {
        sh 'docker build -t aceapp3-openssl-test --file Dockerfile.aceonly .'
      }
    }

  }
}