package cl.julio.tool;

import java.io.IOException;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.List;

public class GenerateAst {

    public static void main(String[] args) throws IOException {
        if (args.length != 1) {
            System.err.println("Usage: generate_ast <output directory>");
            System.exit(64);
        }

        String outputDir = args[0];
        defineAst(outputDir, "Expr", Arrays.asList(
                "Binary   : Expr left, Token operator, Expr right",
                "Grouping : Expr expression",
                "Literal  : Object value",
                "Unary    : Token operator, Expr right"
        ));
    }

    private static void defineAst(String outputDir, String baseName, List<String> types) throws IOException {
        String path = outputDir + "/" + baseName + ".java";
        try (PrintWriter writer = new PrintWriter(path, StandardCharsets.UTF_8)) {
            writer.println("package cl.julio.jlox;");
            writer.println();
            writer.println("import java.util.List;");
            writer.println();
            writer.println("abstract class " + baseName + " {");
            writer.println();

            writer.println("    abstract <R> R accept(Visitor<R> visitor);");
            writer.println();

            // Visitor interface
            writer.println("    interface Visitor<R> {");
            for (String type : types) {
                String typeName = type.split(":")[0].trim();
                writer.println("        R visit" + typeName + baseName + "(" + typeName + " " + baseName.toLowerCase() + ");");
            }
            writer.println("    }");
            writer.println();


            // Expressions classes
            for (String type : types) {
                String className = type.split(":")[0].trim();
                String fieldList = type.split(":")[1].trim();
                String[] fields = fieldList.split(",");

                writer.println("    static class " + className + " extends " + baseName + " {");

                // Fields
                for (String field : fields) {
                    field = field.trim();
                    writer.println("        final " + field + ";");
                }

                writer.println();

                // Constructor
                writer.println("        " + className + "(" + fieldList + ") {");
                for (String field : fields) {
                    String fieldName = field.trim().split(" ")[1].trim();
                    writer.println("            this." + fieldName + " = " + fieldName + ";");
                }
                writer.println("        }");

                // Visitor pattern
                writer.println();
                writer.println("        @Override");
                writer.println("        <R> R accept(Visitor<R> visitor) {");
                writer.println("            return visitor.visit" + className + baseName + "(this);");
                writer.println("        }");

                writer.println("    }");
                writer.println();
            }

            writer.println("}");
        }
    }

}
