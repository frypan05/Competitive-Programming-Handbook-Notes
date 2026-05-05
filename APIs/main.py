from datetime import datetime
from typing import List, Optional

import strawberry
from fastapi import FastAPI
from models import Author, Book, CreateAuthorInput, CreateBookInput
from starlette.types import AppType
from strawberry.fastapi import GraphQLRouter

# Sample data for demonstration
authors_db = [
    Author(
        id=1,
        name="George Orwell",
        email="george@orwell.com",
        bio="English novelist and critic",
        created_at=datetime(2020, 1, 15),
    ),
    Author(
        id=2,
        name="Ray Bradbury",
        email="ray@bradbury.com",
        bio="American author and screenwriter",
        created_at=datetime(2020, 3, 10),
    ),
]

books_db = [
    Book(
        id=1,
        title="1984",
        isbn="978-0-452-28423-4",
        published_year=1949,
        page_count=328,
        author_id=1,
    ),
    Book(
        id=2,
        title="Fahrenheit 451",
        isbn="978-1-451-67331-9",
        published_year=1953,
        page_count=194,
        author_id=2,
    ),
]


@strawberry.type
class User:
    name: str
    age: int


"""
each @strawberry.field is one query.
My queries are:

user() -> User
hello(name: str) -> str
books() -> List[Book]
book(id: int) -> Optional[Book]
authors() -> List[Author]
author(id: int) -> Optional[Author]
"""


"""
Example queries:
query {
  books {
    title
    publishedYear
  }
}
"""


@strawberry.type
class Query:
    @strawberry.field
    def user(self) -> User:
        return User(name="Daksh Sharma", age=22)

    @strawberry.field
    def hello(self, name: str = "World") -> str:
        return f"Hello, {name}! Welcome to Strawberry GraphQL with FastAPI!"

    @strawberry.field
    def books(self) -> List[Book]:
        return books_db

    @strawberry.field
    def book(self, id: int) -> Optional[Book]:
        return next((book for book in books_db if book.id == id), None)

    @strawberry.field
    def authors(self) -> List[Author]:
        return authors_db

    @strawberry.field
    def author(self, id: int) -> Optional[Author]:
        return next((author for author in authors_db if author.id == id), None)


# this is the central point of contract in the API.
# currently it only has query(read) operations but we can add mutations(create, update, delete) here as well.
schema = strawberry.Schema(query=Query)
graphql_app = GraphQLRouter(schema)
app = FastAPI()
app.include_router(graphql_app, prefix="/graphql")
