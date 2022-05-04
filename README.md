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
client_count: 1
key_count: 0
```

```
400 BAD REQUEST
Invalid command.
```

## Responses

- `200 OK`: 
- `400 BAD REQUEST`: 



## Commands


| Command | Arguments              | Example Responses                                             | Description                           |
| ------- | ---------------------- | ------------------------------------------------------------- | ------------------------------------- |
| ADD     | KEY VALUE              |                                                               |                                       |
| APP     | KEY VALUE              |                                                               |                                       |
| BYE     | -                      | ```200 OK```<br/>```bye.```                                   |                                       |
| DEC     | KEY [OFFSET] [INITIAL] |                                                               |                                       |
| DEL     | KEY                    |                                                               |                                       |
| INC     | KEY [OFFSET] [INITIAL] |                                                               |                                       |
| GET     | KEY                    |                                                               |                                       |
| PRE     | KEY VALUE              |                                                               |                                       |
| SET     | KEY VALUE              |                                                               |                                       |
| STT     | -                      | ```200 OK```<br/>```client_count: 1```<br/>```key_count: 0``` | Server key count, memory usage etc... |
| VER     | -                      | ```200 OK```<br/>```1.0```                                    | Server version, key count etc...      |




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
