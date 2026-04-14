import strawberry
from fastapi import FastAPI
from starlette.types import AppType
from strawberry.fastapi import GraphQLRouter


@strawberry.type
class User:
    name: str
    age: int


@strawberry.type
class Query:
    @strawberry.field
    def user(self) -> User:
        return User(name="Daksh", age=22)

    @strawberry.field
    def hello(self, name: str = "World") -> str:
        return f"Hello, {name}! Welcome to Strawberry GraphQL with FastAPI!"


schema = strawberry.Schema(query=Query)
graphql_app = GraphQLRouter(schema)
app = FastAPI()
app.include_router(graphql_app, prefix="/graphql")
