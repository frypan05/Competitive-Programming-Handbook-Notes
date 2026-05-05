# GraphQL with FastAPI (Study Notes)

## What is GraphQL?

GraphQL is a **query language for APIs** and a **runtime** for executing those queries against your data.

Instead of exposing many resource-specific endpoints (as in REST), GraphQL commonly uses a **single endpoint** (like `/graphql`) where clients ask for exactly the fields they need.

In this project context, GraphQL is used with:
- **FastAPI** for the web framework
- **Strawberry** for schema and resolvers
- **Uvicorn** as the ASGI server

---

## Why use GraphQL?

GraphQL is useful when clients need flexibility and efficient data access.

### Key reasons

1. **Fetch exactly what you need**
   - Avoid over-fetching (too much data)
   - Avoid under-fetching (needing multiple requests)

2. **Single endpoint, rich data model**
   - One endpoint can expose many types and relationships

3. **Strongly typed schema**
   - API contract is explicit and self-documenting
   - Better validation and editor tooling

4. **Great developer experience**
   - Tools like GraphiQL provide autocomplete, docs, and schema exploration

5. **Nested and relational queries**
   - Fetch related entities in one request (for example: books with author details)

---

## Core GraphQL properties

A GraphQL API is built from these core building blocks:

1. **Schema**
   - The central contract of your API

2. **Types**
   - Object types like `Author`, `Book`, `User`

3. **Fields**
   - Attributes on types (`name`, `email`, `publishedYear`, etc.)

4. **Arguments**
   - Inputs to fields (`book(id: 1)`)

5. **Queries**
   - Read operations

6. **Mutations**
   - Write operations (create/update/delete)

7. **Input types**
   - Structured payloads passed into mutations

8. **Resolvers**
   - Functions that return field values

9. **Nullability and lists**
   - `Optional[T]` => can return `null`
   - `List[T]` => returns an array/list

10. **Introspection**
   - Clients can inspect schema capabilities dynamically

---

## GraphQL vs REST (quick comparison)

| Aspect | REST | GraphQL |
|---|---|---|
| Endpoints | Multiple | Usually one (`/graphql`) |
| Data shape | Server-defined | Client-defined |
| Over/under-fetching | Common | Reduced |
| Versioning | Often explicit versions | Usually schema evolution |
| Relationships | Multiple calls often needed | Nested in one query |

---

## FastAPI + Strawberry setup flow

## 1) Install dependencies

```/dev/null/commands.sh#L1-1
python -m pip install fastapi "strawberry-graphql[fastapi]" "uvicorn[standard]"
```

## 2) Define schema types

Use `@strawberry.type` for output object types and `@strawberry.input` for mutation inputs.

Example concepts:
- `Author` and `Book` object types
- `CreateAuthorInput`, `CreateBookInput`, `UpdateBookInput` input types

## 3) Create Query and Mutation classes

- `Query` for read fields (`books`, `book`, `authors`, `author`, `hello`, etc.)
- `Mutation` for write fields (`createAuthor`, `createBook`, etc.)

## 4) Build schema and router

- `schema = strawberry.Schema(query=Query, mutation=Mutation)`
- `graphql_app = GraphQLRouter(schema)`
- `app.include_router(graphql_app, prefix="/graphql")`

## 5) Run server

```/dev/null/commands.sh#L1-1
python -m uvicorn main:app --reload
```

Open:
- `http://127.0.0.1:8000/graphql`

---

## Query examples

## Basic query

```/dev/null/graphql-query.graphql#L1-6
{
  user {
    name
    age
  }
}
```

## Nested data query

```/dev/null/graphql-query.graphql#L1-13
{
  books {
    id
    title
    publishedYear
    pageCount
    author {
      name
      bio
    }
  }
}
```

Why this matters:
- Client controls response shape
- Related data can be returned together

---

## Mutation examples

## Create author

```/dev/null/graphql-mutation.graphql#L1-11
mutation {
  createAuthor(input: {
    name: "Isaac Asimov"
    email: "isaac@asimov.com"
    bio: "American science fiction writer"
  }) {
    id
    name
    email
    createdAt
  }
}
```

## Create book

```/dev/null/graphql-mutation.graphql#L1-14
mutation {
  createBook(input: {
    title: "Foundation"
    isbn: "978-0-553-29335-0"
    publishedYear: 1951
    pageCount: 244
    authorId: 3
  }) {
    id
    title
    isbn
    author {
      name
    }
  }
}
```

---

## Resolver design notes

1. **Validation first**
   - Example: verify `author_id` exists before creating a `Book`

2. **Return useful objects**
   - Mutations should return the created/updated entity (or rich result objects)

3. **Relationship resolvers**
   - Add resolvers for nested fields (`Book.author`, `Author.books`)
   - This turns `null` relations into actual linked data when available

4. **Keep type hints accurate**
   - Strawberry relies on Python typing to generate schema correctly

---

## Best practices

1. Use **input objects** for mutation arguments
2. Keep schema names clear and consistent (`createAuthor`, `publishedYear`)
3. Prefer explicit optional fields with `Optional[...]`
4. Raise meaningful exceptions in mutations
5. Separate concerns:
   - `models.py` for GraphQL types/inputs
   - `main.py` for app wiring and resolvers
6. For production:
   - Add database integration (instead of in-memory lists)
   - Add auth/authorization
   - Add pagination and filtering
   - Add proper error handling and logging

---

## Common pitfalls (and fixes)

1. **Import/module errors on startup**
   - Ensure correct run path and module path
   - From `APIs` folder use `main:app`
   - From parent folder use `APIs.main:app`

2. **Wrong package installed**
   - Use `strawberry-graphql`, not unrelated similarly named packages

3. **Editor import warnings**
   - Ensure editor uses the same virtual environment interpreter as terminal

4. **Null relationship fields**
   - Implement relationship resolvers (`author`, `books`) explicitly

---

## Mental model to remember

Think of GraphQL as:
- A **typed contract** (schema)
- A **single flexible API surface**
- A **client-driven query system**

FastAPI + Strawberry gives you:
- High performance
- Clean Python type-based schema definitions
- Easy integration with standard FastAPI routing and middleware

---

## Final takeaway

GraphQL is ideal when clients need flexible, efficient, and strongly typed data access.  
With FastAPI + Strawberry, you can quickly build production-ready GraphQL APIs that are easy to evolve, easy to explore in GraphiQL, and pleasant to use from frontend/mobile clients.

```python
# main.py (minimal setup)
import strawberry
from fastapi import FastAPI
from strawberry.fastapi import GraphQLRouter

@strawberry.type
class User:
    name: str
    age: int

@strawberry.type
class Query:
    @strawberry.field
    def user(self) -> User:
        return User(name="Patrick", age=100)

schema = strawberry.Schema(query=Query)
graphql_app = GraphQLRouter(schema)

app = FastAPI()
app.include_router(graphql_app, prefix="/graphql")
```

```graphql
# example query
{
  user {
    name
    age
  }
}
```
