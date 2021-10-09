# Version Only

This example shows that the LD_AUDIT library doesn't seem to trigger in the presence of the
other, LD_PRELOAD. I was able to get it working at first with just the auditlib here, but
when I added the second preload library, there was no output.

```bash
LD_AUDIT=./auditlib.so LD_PRELOAD=./preloadlib.so whoami
I'm loaded from LD_PRELOAD vanessa
```

I don't have a good reason for this! If you find one, please [let me know](https://github.com/buildsi/ldaudit-yaml/issues).
