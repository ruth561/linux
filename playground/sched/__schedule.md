# __schedule

`__schedule` 関数の実行に必要な時間を測定するため、`__schedule` の最初の部分と `__schedule` の処理が終わった部分に以下のhook関数を用意する。
* `ruth_hook___schedule_entry`
* `ruth_hook___schedule_exit`

これらのhook関数は、`__schedule` の先頭部分とreturnの直前に設置する。ただし、新しいタスクにコンテキストスイッチするときは例外的で、`__schedule` 関数からreturnせずに直接実行を始めることになっている。そのため、`ruth_hook___schedule_exit` はNEWタスクが最初に実行されたときに呼ばれる `schedule_tail` 関数の内部にも設置する。

`__schedule` 関数の呼び出し時はPreemptionがdisabledとなっている。そのため、これらのhook関数の呼び出しの間にPreemptionが起きることはなく、従って、`ruth_hook___schedule_entry` が2回連続で呼び出されるようなことはない。

# __scheduleにかかる時間が計測とともに単調増加する

/sys/kernel/ruth/sched から読み出した統計情報について、平均実行時間が時間が経過するとともに単調増加していた。原因を探る。
⇢ 単純に起動してからの方が `__schedule` の処理が重くなるため、起動直後は単調に増加していただけだった。起動してからある程度時間が経過すると平均の時間もならされていっている。
