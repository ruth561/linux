# __schedule

`__schedule` 関数の実行に必要な時間を測定するため、`__schedule` の最初の部分と `__schedule` の処理が終わった部分に以下のhook関数を用意する。
* `ruth_hook___schedule_entry`
* `ruth_hook___schedule_exit`

これらのhook関数は、`__schedule` の先頭部分とreturnの直前に設置する。ただし、新しいタスクにコンテキストスイッチするときは例外的で、`__schedule` 関数からreturnせずに直接実行を始めることになっている。そのため、`ruth_hook___schedule_exit` はNEWタスクが最初に実行されたときに呼ばれる `schedule_tail` 関数の内部にも設置する。

`__schedule` 関数の呼び出し時はPreemptionがdisabledとなっている。そのため、これらのhook関数の呼び出しの間にPreemptionが起きることはなく、従って、`ruth_hook___schedule_entry` が2回連続で呼び出されるようなことはない。
