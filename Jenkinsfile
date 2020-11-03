pipeline {
environment {
  registry = "fenglin1/aceapp3-base"
  registryCredential = 'dockerhub'
}
agent any
stages {
  stage('Building image') {
    steps{
      script {
        docker.build registry + ":$BUILD_NUMBER"
      }
    }
  }
stage('Deploy Image') {
    steps{
      script {
        docker.withRegistry( '', registryCredential ) {
          dockerImage.push()
        }
      }
    }
  }
  stage('Remove Unused docker image') {
    steps{
      sh "docker rmi $registry:$BUILD_NUMBER"
    }
  }
}
}
