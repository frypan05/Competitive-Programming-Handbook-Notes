variable "project_name" {
  type        = string
  default     = "ops-blog"
  description = "Logical name for tagging and container identification."
}

variable "environment" {
  type        = string
  default     = "dev"
  description = "Environment label used for tagging (e.g., dev/stage/prod)."
}

variable "owner" {
  type        = string
  default     = "devops-demo"
  description = "Owner/Team tag to apply to managed resources."
}

variable "docker_host" {
  type        = string
  default     = null
  description = "Docker host endpoint. Example: unix:///var/run/docker.sock (Linux/macOS) or npipe:////./pipe/docker_engine (Windows). Null uses provider default."
}

variable "image_name" {
  type        = string
  default     = "ops-blog"
  description = "Docker image name to deploy."
}

variable "image_tag" {
  type        = string
  default     = "local"
  description = "Docker image tag to deploy (e.g., local, latest, or a CI build tag)."
}

variable "container_name" {
  type        = string
  default     = "ops-blog-demo"
  description = "Name of the running container instance."
}

variable "host_port" {
  type        = number
  default     = 8080
  description = "Host port to bind for the web service."
}

variable "container_port" {
  type        = number
  default     = 80
  description = "Container port exposed by the image (Nginx default)."
}

variable "restart_policy" {
  type        = string
  default     = "unless-stopped"
  description = "Docker restart policy for the container."
}
