# Vix Cache

Offline-first HTTP cache • Deterministic policy • Memory + File stores • LRU eviction • Cache keys • Pruning

The Vix **cache** module provides a small, fast, predictable caching layer designed for **offline-first runtimes**.

It is built around a few explicit primitives:

* `Cache` — the main entry point
* `CachePolicy` — deterministic caching rules (TTL, stale windows)
* `CacheContext` — request context (online/offline/network-error)
* Stores: `MemoryStore`, `FileStore`, `LruMemoryStore`
* `CacheKey` — stable key builder (normalizes query + optional vary headers)

This module is used by higher layers (e.g. HTTP GET cache middleware), but it is intentionally usable as a **standalone library**.

---

## Why this module exists

In offline-first systems, caching is not just a performance optimization.
It is part of **correctness under failure**:

* if the device is **offline**, you must be able to serve previously validated data
* if the network is **unstable**, you must be able to fall back to stale data safely
* behavior must be **observable**, explicit, and testable

This module makes all caching decisions explicit with:

* a `CachePolicy`
* a `CacheContext`
* a time `t_ms`

So you can test cache behavior as pure logic.

---

## Quick start (run the smoke tests)

The module includes runnable smoke tests (copy/paste friendly examples).

```bash
vix run modules/cache/tests/cache_smoke_test.cpp
vix run modules/cache/tests/cache_context_mapper_smoke_test.cpp
```

(Exact paths may differ in your tree; the files shown below are the reference examples.)

---

## Core concepts

### 1) CachePolicy

`CachePolicy` defines deterministic time windows:

* `ttl_ms` — fresh window (normal caching)
* `allow_stale_if_offline` + `stale_if_offline_ms` — stale window when offline
* `allow_stale_if_error` + `stale_if_error_ms` — stale window on network errors

Example:

```cpp
vix::cache::CachePolicy policy;
policy.ttl_ms = 100;
policy.allow_stale_if_offline = true;
policy.stale_if_offline_ms = 10'000;
policy.allow_stale_if_error = true;
policy.stale_if_error_ms = 5'000;
```

---

### 2) CacheContext

The same cached entry may be acceptable or rejected depending on the context:

* `CacheContext::Online()`
* `CacheContext::Offline()`
* `CacheContext::NetworkError()`

Example:

```cpp
auto got = cache.get(key, t_now, vix::cache::CacheContext::Offline());
```

This is how you express: “accept stale data when offline, but not forever”.

---

### 3) Stores

The cache separates **policy** from **storage**.

#### MemoryStore

* in-memory map
* fastest
* ideal for servers / ephemeral caching

#### FileStore

* persists cache to disk
* reloads on startup
* suitable for local-first apps and edge runtimes

#### LruMemoryStore

* in-memory LRU eviction
* bounded size via `max_entries`
* good for servers that must cap memory

---

## Minimal usage

```cpp
#include <memory>
#include <vix/cache/Cache.hpp>
#include <vix/cache/CacheEntry.hpp>
#include <vix/cache/CachePolicy.hpp>
#include <vix/cache/CacheContext.hpp>
#include <vix/cache/MemoryStore.hpp>

int main()
{
  using namespace vix::cache;

  auto store = std::make_shared<MemoryStore>();

  CachePolicy policy;
  policy.ttl_ms = 10'000;
  policy.allow_stale_if_offline = true;
  policy.stale_if_offline_ms = 60'000;

  Cache cache(policy, store);

  const std::string key = "GET:/api/users?page=1";
  const std::int64_t t0 = 1000; // example time

  CacheEntry e;
  e.status = 200;
  e.body = R"({\"ok\":true})";
  e.created_at_ms = t0;

  cache.put(key, e);

  auto got = cache.get(key, t0 + 5, CacheContext::Online());
  if (got) {
    // use got->status / got->body / got->headers
  }
}
```

---

## Offline-first behavior (from the context mapper smoke test)

The recommended pattern is:

1. Determine connectivity context (online/offline/error)
2. If **offline** → cache only
3. If **online** → attempt network
4. If **network error** → fall back to cache with `NetworkError` context

This logic is demonstrated as a testable pure function in:

* `cache_context_mapper_smoke_test.cpp`

It validates the key offline-first behaviors:

* offline + cached entry → **CacheHit**
* offline + no entry → **OfflineMiss**
* online + network ok → **NetOk** + cache populated
* online + network error → **CacheHit** if `allow_stale_if_error`

---

## FileStore persistence

The file store smoke test validates:

* entries persist to disk
* new cache instance can reload the data
* offline stale rules still apply after reload

Example config (conceptual):

```cpp
vix::cache::FileStore::Config cfg;
cfg.file_path = "./build/.vix_test/cache_http.json";
cfg.pretty_json = true;

auto store = std::make_shared<vix::cache::FileStore>(cfg);
```

---

## Header normalization

When inserting entries, the cache normalizes header keys to **lower-case**.

This ensures consistent lookup and avoids duplicated keys (case variants).

The smoke test verifies:

* `Content-Type` becomes `content-type`
* `X-Powered-By` becomes `x-powered-by`

---

## LRU eviction

`LruMemoryStore` supports eviction via `max_entries`.

The smoke test demonstrates:

* LRU behavior is stable
* touching an entry updates its recency
* inserting beyond capacity evicts the least recently used key

Example:

```cpp
auto store = std::make_shared<vix::cache::LruMemoryStore>(
  vix::cache::LruMemoryStore::Config{.max_entries = 2048}
);
```

---

## Pruning stale entries

`Cache::prune(t_ms)` removes entries that are too old under the active policy.

The test demonstrates:

* stale entries removed
* fresh entries kept

This is useful for:

* periodic cleanup loops
* memory-bounded caches
* long-running edge runtimes

---

## CacheKey builder

`CacheKey::fromRequest()` generates stable cache keys by:

* normalizing query params order (`b=2&a=1` → `a=1&b=2`)
* optionally varying on selected headers (`Accept`, etc.)

Example:

```cpp
std::unordered_map<std::string, std::string> headers;
headers["Accept"] = "application/json";
headers["X-Device"] = "mobile";

auto k = vix::cache::CacheKey::fromRequest(
  "GET",
  "/api/users",
  "b=2&a=1",
  headers,
  {"Accept"}
);
```

This is critical to avoid cache fragmentation and to keep behavior deterministic.

---

## How cache fits in the umbrella

* `cache` provides the offline-first caching core
* `middleware` builds HTTP GET caching on top (control headers, debug headers, bypass)
* `sync` and offline engines can reuse the same policy/context model

---

## Directory layout

Typical layout:

```
modules/cache/
│
├─ include/vix/cache/
│  ├─ Cache.hpp
│  ├─ CacheEntry.hpp
│  ├─ CachePolicy.hpp
│  ├─ CacheContext.hpp
│  ├─ CacheContextMapper.hpp
│  ├─ CacheKey.hpp
│  ├─ MemoryStore.hpp
│  ├─ FileStore.hpp
│  ├─ LruMemoryStore.hpp
│  └─ ...
│
└─ tests/
   ├─ cache_smoke_test.cpp
   └─ cache_context_mapper_smoke_test.cpp
```

---

## License

MIT — same as Vix.cpp

Repository: [https://github.com/vixcpp/vix](https://github.com/vixcpp/vix)
