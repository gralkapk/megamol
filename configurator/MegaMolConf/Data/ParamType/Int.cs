﻿namespace MegaMolConf.Data.ParamType {

    public sealed class Int : ParamTypeValueBase {
        public int DefaultValue { get; set; }
        public int MinValue { get; set; }
        public int MaxValue { get; set; }
        public override void ParseDefaultValue(string v) {
            DefaultValue = int.Parse(v);
        }
        public override string DefaultValueString() {
            return DefaultValue.ToString();
        }
        public override bool ValuesEqual(string a, string b) {
            return int.Parse(a).Equals(int.Parse(b));
        }
    }

}
