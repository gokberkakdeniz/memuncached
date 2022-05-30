# memuncached

_memuncached_ is the worst undistributed memory object caching system.

## Protocol

## Response

### Format

```CODE DESCRIPTION\r\nLENGTH\r\nPAYLOAD\r\n\0```

### Examples

```
200 OK
0
```

```
200 OK
3
1.0
```

```
200 OK
28
Client-Count: 1
Key-Count: 0
```

```
400 BAD REQUEST
16
Invalid command.
```

## Responses

- `200 OK`: 
- `400 BAD REQUEST`: 
- `404 NOT FOUND`:
- `404 KEY EXIST`:


## Commands


| Command | Arguments                  | Example Responses                                             | Description                                                                                |
| ------- | -------------------------- | ------------------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| ADD     | KEY TYPE LENGTH<br/>VALUE  | <pre>404 KEY EXIST<br/>0</pre>                                | Similar to SET but fails if key exists.                                                    |
| APP     | KEY LENGTH<br/>VALUE       | <pre></pre>                                                   |                                                                                            |
| BYE     | -                          | <pre>200 OK<br/>4<br/>bye.</pre>                              | Close connection.                                                                          |
| DEC     | KEY [OFFSET=1] [INITIAL=1] | <pre>200 OK<br/>1<br/>1</pre>                                 | Decrease the value of KEY by OFFSET. If the value is empty INITIAL value will be inserted. |
| DEL     | KEY                        | <pre>200 OK<br/>5<br/>value</pre>                             | Deletes the value of the key and returns the value if exist.                               |
| INC     | KEY [OFFSET=1] [INITIAL=1] | <pre>200 OK<br/>3<br/>3.5</pre>                               | Increase the value of KEY by OFFSET. If the value is empty INITIAL value will be inserted. |
| GET     | KEY                        | <pre>200 OK<br/>4<br/>a b\nc</pre>                            | Get the value of the key.                                                                  |
| PRE     | KEY LENGTH<br/>VALUE       | <pre></pre>                                                   |                                                                                            |
| SET     | KEY TYPE LENGTH<br/>VALUE  | <pre>200 OK<br/>0</pre>                                       | Set the value of the key.                                                                  |
| STT     | -                          | <pre>200 OK<br/>28<br/>Client-Count: 1<br/>Key-Count: 0</pre> | Get total key count, client count etc...                                                   |
| VER     | -                          | <pre>200 OK<br/>3<br/>1.0</pre>                               | Get server version.                                                                        |

STT VER BYE DEC INC DEL SET GET ADD


TODO: KEY VALIDATION

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
