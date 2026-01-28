#############################################
# Docker-based deployment (zero-cost, local)
#############################################

locals {
  full_image_name = "${var.image_name}:${var.image_tag}"
}

# Build the image from the project root (one level up from terraform/)
resource "docker_image" "ops_blog" {
  name         = local.full_image_name
  keep_locally = true

  build {
    context    = "../"
    dockerfile = "../Dockerfile"
    # You can add build args here if needed:
    # build_arg = { EXAMPLE = "value" }
  }
}

# Run the container and bind host port -> container port
resource "docker_container" "ops_blog" {
  name  = var.container_name
  image = docker_image.ops_blog.image_id

  ports {
    internal = var.container_port
    external = var.host_port
    protocol = "tcp"
  }

  restart = var.restart_policy

  # Example healthcheck; adjust to your needs
  healthcheck {
    test     = ["CMD", "wget", "-qO-", "http://localhost:${var.container_port}"]
    interval = "30s"
    timeout  = "3s"
    retries  = 3
  }

  # Mounts / volumes can be added if you want to live-edit site content:
  # mounts {
  #   target = "/usr/share/nginx/html"
  #   source = "/absolute/path/to/site"
  #   type   = "bind"
  #   read_only = false
  # }

  # Environment variables (not used for static site, but here for demonstration):
  # env = [
  #   "APP_ENV=${var.environment}",
  #   "PROJECT_NAME=${var.project_name}"
  # ]
}

output "container_name" {
  value       = docker_container.ops_blog.name
  description = "Name of the running container."
}

output "service_url" {
  value       = "http://localhost:${var.host_port}"
  description = "Local URL to access the static blog."
}

output "image_tagged" {
  value       = local.full_image_name
  description = "Image tag used for this deployment."
}
