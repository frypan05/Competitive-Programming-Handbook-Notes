# Containerization

---

### I. Building and Pushing Docker Images

In modern DevOps, compiling code (e.g., into a `.jar` file) is usually followed by containerization to ensure portability across environments.

* **Execution Options:**
  * **Manual Commands (`run`):** You can manually write the shell commands for `docker login`, `docker build`, `docker tag`, and `docker push`.
  * **Pre-defined Actions (`uses`):** It is more efficient to use a verified action from the **GitHub Marketplace**. This abstracts the complex logic of authentication and multi-platform tagging.
* **The Docker Environment:**
  * GitHub's **Ubuntu runners** come with Docker pre-installed.
  * [**Context Note:** If using a Windows or macOS runner, Docker may not be natively available or may require additional setup steps/actions to initialize.]
* **Multi-line Commands:** When writing custom shell scripts in a step, use the **pipe (`|`)** symbol in YAML to group multiple commands (e.g., logging in and then building).

### II. Security: Secrets Management

Because workflow `.yml` files are stored in plain text within the repository, sensitive data (passwords, API keys, tokens) must never be hardcoded.

* **GitHub Secrets:** A built-in feature to store sensitive information securely at the repository level.
* **Configuration Path:** `Settings` -> `Secrets and variables` -> `Actions` -> `New repository secret`.
* **Referencing Secrets:** Secrets are accessed via a specific syntax that GitHub parses at runtime.
  * **Syntax:** `${{ secrets.SECRET_NAME }}`
* **Masking:** GitHub automatically masks secrets in the workflow logs. If a command prints a secret to the console, it will appear as `***`.

### III. Hands-on Demo: Pushing to Docker Hub

The tutorial demonstrates pushing a Java Gradle application to a private Docker Hub repository.

* **Required Parameters for Docker Actions:**
  * **Registry:** For Docker Hub, the registry address is `docker.io`.
  * **Image Name:** Usually follows the format `username/repository-name`.
  * **Credentials:** Requires a username and password (or Access Token) stored in GitHub Secrets.
* **The Build/Push Process:**
    1. **Authentication:** The action performs a `docker login` using the provided secrets.
    2. **Context:** The action looks for a `Dockerfile` in the root directory by default.
    3. **Tagging:** By default, many actions tag the image with the branch name (e.g., `master`). This can be overridden using a `tags` parameter to support versioning (e.g., `v1.0.1` or the specific Git commit SHA).

### IV. The Complete CI Pipeline Summary

A standard Continuous Integration pipeline in GitHub Actions follows this logical flow:

1. **Trigger:** Developer pushes code or creates a Pull Request.
2. **Environment Setup:** A runner is provisioned; Java/JDK is installed.
3. **Build/Test:** The code is compiled (Gradle/Maven) and unit tests are executed.
4. **Containerization:** The resulting artifact is wrapped in a Docker image.
5. **Distribution:** The image is pushed to a private or public registry (Docker Hub, AWS ECR, etc.), making it ready for deployment.

---

### Key Takeaways

1. **Marketplace Power:** For common tasks like Docker integration, using community-vetted actions is safer and faster than writing custom shell scripts.
2. **"Secret" Integrity:** Always use `${{ secrets.NAME }}` to keep your credentials out of your source code.
3. **Infrastructure as Code:** Your entire CI/CD pipeline is version-controlled alongside your application code, ensuring that the build process evolves with the software.
4. **Default Tooling:** Ubuntu runners are the "workhorse" of GitHub Actions because they include pre-installed tools like Docker, Git, and various CLI utilities.
