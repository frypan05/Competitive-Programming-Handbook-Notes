terraform {
  required_version = ">= 1.5.0"

  required_providers {
    docker = {
      source  = "kreuzwerker/docker"
      version = "~> 3.0"
    }
  }
}

provider "docker" {
  host = var.docker_host # e.g., "unix:///var/run/docker.sock" or "npipe:////./pipe/docker_engine" on Windows
}
