function ARFieldDataTypeToString(dataType) {
    switch (dataType) {
        case 0: return "Null";
        case 1: return "Keyword";
        case 2: return "Integer";
        case 3: return "Real";
        case 4: return "Character";
        case 5: return "Diary";
        case 6: return "Selection";
        case 7: return "Date/Time";
        case 8: return "Bitmask";
        case 9: return "Bytes";
        case 10: return "Decimal";
        case 11: return "Attach";
        case 12: return "Currency";
        case 13: return "Date";
        case 14: return "Time of Day";
        case 30: return "Join";
        case 31: return "Trim";
        case 32: return "Control";
        case 33: return "Table";
        case 34: return "Column";
        case 35: return "Page";
        case 36: return "Page Holder";
        case 37: return "Attach Pool";
        case 40: return "Long";
        case 41: return "Coords";
        case 42: return "View";
        case 43: return "Display";
        default: return "unknwon";
    };
};