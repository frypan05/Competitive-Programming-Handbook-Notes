Rest centers around resources, while GraphQL centers around data. 
In REST, you have multiple endpoints for different resources, and you often need to make multiple requests to fetch related data. 
In GraphQL, you can fetch all the related data in a single request by specifying exactly what you need.

For example of a book = author, in REST you might have:
- GET /books/1 -> returns book details (title, publishedYear, etc.)
- GET /books/1/author -> returns author details (name, email, etc.)
- In GraphQL, you can do:
```
query {
  book(id: 1) {
    title
    publishedYear
    author {
      name
      email
    }
  }
}
```

```
type Book {
  id: ID
  title: String
  publishedYear: Int
```
This query fetches the book details and the related author details in one request, avoiding multiple round-trips to the server.

Rest Client:
GET /books/1 -> returns book details
GET /books/1/author -> returns author details

GraphQL Client:
GET /graphql?query={book(id: 1) {title, publishedYear, author { name, email}}}


GRAPHQLs doesn't use urls to specify the resource available in the API, instead it uses a (graphql schema) single endpoint and the query specifies what data is needed.
