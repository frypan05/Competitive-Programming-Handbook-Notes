# Workflow Syntax

---

## I. Understanding Action Execution (`uses` vs. `run`)

A workflow job consists of a sequence of steps. These steps typically do one of two things: execute a pre-defined action or run a shell command.

* **The `uses` Attribute:**
  * **Purpose:** Refers to a reusable, pre-defined action hosted in a GitHub repository.
  * **The "Marketplace" Concept:** Most common tasks (like checking out code or setting up a language environment) are already written by GitHub or the community.
  * **Syntax:** `uses: actions/checkout@v4`
    * `actions/checkout`: The path to the repository containing the action logic.
    * `@v4`: The version tag (it is best practice to use versioning to ensure pipeline stability).
  * **Custom/Community Actions:** You can use any action by referencing its repository path, provided it contains an `action.yml` file defining its logic.

* **The `run` Attribute:**
  * **Purpose:** Executes a raw command-line script on the runner's operating system.
  * **Multi-line Syntax:** Use the pipe symbol (`|`) to execute multiple commands in a single step.
  * **Example:**

        ```yaml
        - name: Build with Gradle
          run: ./gradlew build
        ```

## II. The GitHub Action Runner (Infrastructure)

* **Managed Execution:** Workflows run on **GitHub-hosted runners**—servers managed by GitHub. This eliminates the need for users to maintain build servers or install plugins.
* **Isolation Protocol:**
  * For every **Job**, a fresh, clean virtual machine is provisioned.
  * Once the job finishes, the server is discarded. This ensures no "configuration drift" or leftover files from previous builds.
* **Operating System Options:** Users can specify the environment using the `runs-on` key. Supported OS include:
  * **Ubuntu** (Linux)
  * **Windows**
  * **macOS**

## III. Advanced Job Orchestration

By default, all jobs in a workflow file run in **parallel** to save time. However, dependencies often require a specific order.

* **Sequential Execution (`needs`):**
  * Used to specify that a job depends on the successful completion of another.
  * **Example Case:** A "Publish" job should only run *if* the "Build" job passes.
  * **Syntax:**

        ```yaml
        jobs:
          build: ...
          publish:
            needs: build
            runs-on: ubuntu-latest
        ```

* **The Matrix Strategy (`strategy: matrix`):**
  * **Purpose:** Allows you to run the same job across multiple configurations (e.g., different OS or language versions) simultaneously.
  * **Efficiency:** Instead of writing three separate jobs for Linux, Windows, and Mac, you define a matrix.
  * **Syntax Example:**

        ```yaml
        strategy:
          matrix:
            os: [ubuntu-latest, windows-latest, macos-latest]
        runs-on: ${{ matrix.os }}
        ```

  * [**Context Note:** This creates a "Fan-out" effect where GitHub Actions automatically spawns a separate job instance for every combination in the matrix.]

## IV. Practical Example: Java Gradle Build Workflow

The transcript details a standard Continuous Integration (CI) flow for a Java project:

1. **Event Trigger:** `push` or `pull_request` to the `master` branch.
2. **Step 1: Checkout:** Uses `actions/checkout` to pull the code onto the runner.
3. **Step 2: Setup JDK:** Uses `actions/setup-java` to install a specific Java version (e.g., 1.8/8).
    * [**Context Note:** The `with` keyword is used here to pass inputs to the action, such as `java-version: '11'` or `distribution: 'adopt'`.]
4. **Step 3: Permissions:** Runs `chmod +x gradlew` to ensure the Gradle wrapper is executable.
5. **Step 4: Build:** Runs `./gradlew build` to compile code and run tests.

---

### Key Takeaways

1. **Modularity:** Use `uses` for complex setup tasks and `run` for project-specific shell commands.
2. **Parallelism vs. Dependency:** Use the `needs` keyword to transform parallel jobs into a sequential pipeline.
3. **Multi-Platform Testing:** The Matrix Strategy is the most efficient way to ensure cross-platform compatibility without duplicating code.
4. **Fresh Environments:** Every job starts with a "blank slate" server, preventing "it works on my machine" issues in CI.
