# memuncached

_memuncached_ is the worst undistributed memory object caching system.

## Must To Do

- [ ] getStats(): array|false
- [ ] getVersion(): array
- [ ] add(string $key, mixed $value, int $expiration = ?): bool
- [ ] get(string $key, callable $cache_cb = ?, int $$flags = ?): mixed
- [ ] decrement(string $key, int $offset = 1, int $initial_value = 0, int $expiry = 0): int|false
- [ ] increment(string $key, int $offset = 1, int $initial_value = 0, int $expiry = 0): int|false
- [ ] delete(string $key, int $time = 0): bool
- [ ] quit(): bool
- [ ] set(string $key, mixed $value, int $expiration = ?): bool

| Command | Arguments              | Returns | Description                           |
| ------- | ---------------------- | ------- | ------------------------------------- |
| STAT    | -                      |         | Server key count, memory usage etc... |
| VERSION | -                      |         | Server version, key count etc...      |
| HELP    | -                      |         |                                       |
| ADD     | KEY VALUE              |         |                                       |
| GET     | KEY                    |         |                                       |
| SET     | KEY VALUE              |         |                                       |
| INC     | KEY [OFFSET] [INITIAL] |         |                                       |
| DEC     | KEY [OFFSET] [INITIAL] |         |                                       |
| DEL     | KEY                    |         |                                       |
| BYE     | -                      |         |                                       |

## Nice To Do - Level 1

- [ ] flush(int $delay = 0): bool
- [ ] touch(string $key, int $expiration): bool
- [ ] append(string $key, string $value): bool
- [ ] prepend(string $key, string $value): bool
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
