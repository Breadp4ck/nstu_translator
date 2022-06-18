void Parser::parse(std::vector<Token> tokens) {
    // ...

    // For RGZ
    enum NumberType {
        NUMBER_INTEGER,
        NUMBER_FLOAT
    };

    NumberType rExpr = NUMBER_INTEGER;
    NumberType lExpr = NUMBER_INTEGER;

    // ...

    for (auto& token : tokens) {

        // ...

        while (true) {

            // ...
            
            // Choose temp type
            if (currentRow == 27)       tempTypeID = TYPE_INT;
            else if (currentRow == 28)  tempTypeID = TYPE_CHAR;
            else if (currentRow == 29)  tempTypeID = TYPE_FLOAT;
            else if (currentRow == 23)  tempTypeID = TYPE_NULL;

            // ...

            if (row.accept_) {
                switch (token.tableID) {
                    case TABLE_CONSTANTS:
                        // ...
                        break;

                    case TABLE_VARIABLES:
                        var = tables.variables->ByHash(token.rowID);

                        // ...
                        
                        if (var.type() == TYPE_FLOAT) {
                            if (tempCurrentRow == 30 || tempCurrentRow == 47) {
                                lExpr = NUMBER_FLOAT;

                            } else {
                                rExpr = NUMBER_FLOAT;
                            }
                        }

                        break;

                    case TABLE_SEPARATORS:
                        if (lExpr == NUMBER_INTEGER && rExpr == NUMBER_FLOAT) {
                            ParserError err = { "Невозможно приравнять вещественное число к целому", (int) currentLine };
                            addError(err);
                            return;
                        }

                        lExpr = NUMBER_INTEGER;
                        rExpr = NUMBER_INTEGER;

                        break;
                    
                    case TABLE_OPERATIONS:
                        // ...
                        break;
                }

                // ...

                break;
            }
        }

        currentTokenNumber++;
    }
    
    // ...
}
