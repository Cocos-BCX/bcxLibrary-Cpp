//
//  ViewController.m
//  BCXTestMac
//
//  Created by hugo on 2019/12/27.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#import "ViewController.h"
#import "../../Classes/BCXTest.hpp"

static BCXTest BT;

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    
//    BT.setNativeLog([self](const std::string& s) {
//        [self showLog:[NSString stringWithUTF8String:s.c_str()]];
//    });
//    BT.init();
//    [self showTestCases];
//    [NSTimer scheduledTimerWithTimeInterval:0.1
//                                    repeats:YES
//                                      block:^(NSTimer * _Nonnull timer) {
//        BT.loop();
//    }];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (void)showTestCases {
    int x = 60, y = 50;
//    const auto testCasesCount = BT.getTestCasesCount();
//    for (int i = 0; i < testCasesCount; i++) {
//        UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
//        [button setTitle:[NSString stringWithUTF8String:BT.getTestCasesName(i).c_str()] forState:UIControlStateNormal];
//        button.center = CGPointMake(x, y);
//        [button sizeToFit];
//        [button addTarget:self action:@selector(testCasePressed:) forControlEvents:UIControlEventTouchUpInside];
//        [self.view addSubview:button];
//
//        y = y + 30;
//    }
}

//- (void)testCasePressed:(UIButton *)button {
//    NSLog(@"Test:%@", button.titleLabel.text);
//    [self showLog: [NSString stringWithFormat:@"Test:%@", button.titleLabel.text] ];
//    BT.runTestCase(button.titleLabel.text.UTF8String);
//}
//
//- (void)showLog:(NSString*)s {
//    if (nullptr == self.txtLog) {
//        return;
//    }
//    NSString *txt = self.txtLog.text;
//    NSArray *sArr = [txt componentsSeparatedByString:@"\n"];
//    NSMutableArray *mutArr = [NSMutableArray arrayWithArray:sArr];
//    while (mutArr.count > 3) {
//        [mutArr removeObjectAtIndex:0];
//    }
//    [mutArr addObject:s];
//    txt = [mutArr componentsJoinedByString:@"\n"];
//    self.txtLog.text = txt;
//}

@end
