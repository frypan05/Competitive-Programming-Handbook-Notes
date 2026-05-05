from datetime import datetime
from typing import List, Optional

import strawberry


@strawberry.type
class Author:
    id: int
    name: str
    email: str
    bio: Optional[str] = None
    created_at: datetime
    books: List["Book"] = strawberry.field(default_factory=list)


@strawberry.type
class Book:
    id: int
    title: str
    isbn: str
    published_year: int
    page_count: int
    author_id: int
    author: Optional[Author] = None


@strawberry.input
class CreateAuthorInput:
    name: str
    email: str
    bio: Optional[str] = None


@strawberry.input
class CreateBookInput:
    title: str
    isbn: str
    published_year: int
    page_count: int
    author_id: int


@strawberry.input
class UpdateBookInput:
    title: Optional[str] = None
    isbn: Optional[str] = None
    published_year: Optional[int] = None
    page_count: Optional[int] = None
