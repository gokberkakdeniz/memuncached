# memuncached

_memuncached_ is the worst undistributed memory object caching system.

## Protocol

## Response

### Format

```CODE DESCRIPTION\r\nMESSAGE\r\n\0```

### Examples

```
200 OK
```

```
200 OK
1.0
```

```
200 OK
Client-Count: 1
Key-Count: 0
```

```
400 BAD REQUEST
Invalid command.
```

## Responses

- `200 OK`: 
- `400 BAD REQUEST`: 



## Commands


| Command | Arguments              | Example Responses                                      | Description                              |
| ------- | ---------------------- | ------------------------------------------------------ | ---------------------------------------- |
| ADD     | KEY VALUE              | <pre></pre>                                            |                                          |
| APP     | KEY VALUE              | <pre></pre>                                            |                                          |
| BYE     | -                      | <pre>200 OK<br/>bye.</pre>                             | Close connection.                        |
| DEC     | KEY [OFFSET] [INITIAL] | <pre></pre>                                            |                                          |
| DEL     | KEY                    | <pre></pre>                                            |                                          |
| INC     | KEY [OFFSET] [INITIAL] | <pre></pre>                                            |                                          |
| GET     | KEY                    | <pre></pre>                                            |                                          |
| PRE     | KEY VALUE              | <pre></pre>                                            |                                          |
| SET     | KEY VALUE              | <pre></pre>                                            |                                          |
| STT     | -                      | <pre>200 OK<br/>Client-Count: 1<br/>Key-Count: 0</pre> | Get total key count, memory usage etc... |
| VER     | -                      | <pre>200 OK<br/>1.0</pre>                              | Get server version.                      |




## Nice To Do - Level 1

- [ ] flush(int $delay = 0): bool
- [ ] touch(string $key, int $expiration): bool
- [ ] getAllKeys(): array|false
- [ ] getMulti(array $keys, int $flags = ?): mixed
- [ ] deleteMulti(array $keys, int $time = 0): array
- [ ] setMulti(array $items, int $expiration = ?): bool
- [ ] replace(string $key, mixed $value, int $expiration = ?): bool

## Nice To Do - Level 2

- [ ] cas
- [ ] getDelayed
- [ ] fetch
- [ ] fetchAll
