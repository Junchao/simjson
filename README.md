# Simjson

## 简单说明

C实现的json编码/解码器。

/examples包含了简单的encode/decode例子。

/include包含了暴露的API。

## TODO

* string类型，没有考虑unicode。

* number类型的编码/解码有待完善。

* object类型采用了固定容量的哈希表，需要考虑负载因子较大的情况。

* 单元测试存在内存泄漏问题。

* 需要更多的测试用例。

* 重写cMakeLists。